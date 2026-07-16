#include "reflection.h"
#include "compile_commands.h"
#include <cassert>
#include <clang-c/Index.h>
#include <fstream>
#include <iostream>

namespace reflection
{
namespace
{
    // AST巡回時のコールバック関数
    CXChildVisitResult visitor(CXCursor cursor, CXCursor parent, CXClientData clientData)
    {
        CXCursorKind kind = clang_getCursorKind(cursor);

        // 1. クラスまたは構造体を見つけた場合
        if(kind == CXCursor_ClassDecl || kind == CXCursor_StructDecl) {
            ClassInfo* classInfo = static_cast<ClassInfo*>(clientData);

            CXString nameStr = clang_getCursorSpelling(cursor);
            classInfo->className_ = clang_getCString(nameStr);
            clang_disposeString(nameStr);

            // クラスの内部（子ノード）をさらに巡回する
            clang_visitChildren(cursor, visitor, classInfo);
            return CXChildVisit_Continue;
        }

        // 2. メンバ変数を見つけた場合
        if(kind == CXCursor_FieldDecl) {
            ClassInfo* classInfo = static_cast<ClassInfo*>(clientData);

            // 変数に属性（アノテーション）がついているかチェック
            bool hasMyProperty = false;
            clang_visitChildren(cursor, [](CXCursor subCursor, CXCursor p, CXClientData data) {
                if(clang_getCursorKind(subCursor) == CXCursor_AnnotateAttr) {
                    CXString annotation = clang_getCursorSpelling(subCursor);
                    if(std::string(clang_getCString(annotation)) == "my_property") {
                        *static_cast<bool*>(data) = true;
                    }
                    clang_disposeString(annotation);
                }
                return CXChildVisit_Continue;
            },
                                &hasMyProperty);

            // 目印がついていたら、型名と変数名を取得して保存
            if(hasMyProperty) {
                CXString nameStr = clang_getCursorSpelling(cursor);
                CXType type = clang_getCursorType(cursor);
                CXString typeStr = clang_getTypeSpelling(type);

                classInfo->properties_.push_back({clang_getCString(typeStr),
                                                  clang_getCString(nameStr)});

                clang_disposeString(nameStr);
                clang_disposeString(typeStr);
            }
        }
        return CXChildVisit_Continue;
    }

    void generateEditorCode(const ClassInfo& classInfo)
    {
        std::ofstream out(classInfo.className_ + "_generated.h");

        out << "// 自動生成されたコードです。手動で編集しないでください。\n";
        out << "#pragma once\n\n";
        out << "template<typename T>\n";
        out << "void drawInspector(T& obj);\n\n";

        // 特定のクラスに対するテンプレート特殊化関数を生成
        out << "template<>\n";
        out << "inline void drawInspector<" << classInfo.className_ << ">(" << classInfo.className_ << "& obj) {\n";

        for(const auto& prop: classInfo.properties_) {
            // 例: 汎用的なGUI関数を呼ぶコードを生成（ImGuiなどを想定）
            out << "    drawPropertyField(\"" << prop.name_ << "\", obj." << prop.name_ << ");\n";
        }

        out << "}\n";
        std::cout << classInfo.className_ << "_generated.h を出力しました。\n";
    }
} // namespace

void Reflection::parse(const char* filepath, const char* compile_commands)
{
    assert(nullptr != filepath);
    std::vector<std::string> args_storage;
    std::vector<const char*> clang_args;
    if(nullptr != compile_commands) {
        args_storage = get_args(compile_commands);
        clang_args.reserve(args_storage.size());
        for(size_t i = 0; i < args_storage.size(); ++i) {
            clang_args.push_back(args_storage[i].c_str());
        }
    }

    CXIndex index = clang_createIndex(0, 0);
    CXTranslationUnit translationUnit;

    enum CXErrorCode parse_err = clang_parseTranslationUnit2(
        index, filepath,
        clang_args.data(), clang_args.size(),
        nullptr, 0, CXTranslationUnit_None, &translationUnit);

    clang_disposeIndex(index);
    if(parse_err != CXError_Success) {
        std::cout << "パース失敗" << std::endl;
        clang_disposeTranslationUnit(translationUnit);
        return;
    }

    // パース（前回の質問のコードなど）で tu が取得できている前提
    CXCursor rootCursor = clang_getTranslationUnitCursor(translationUnit);

    ClassInfo classData;
    clang_visitChildren(rootCursor, visitor, &classData);
    clang_disposeTranslationUnit(translationUnit);

    // 抽出されたデータがあれば、エディタ用ヘッダーを自動生成
    if(!classData.className_.empty() && !classData.properties_.empty()) {
        generateEditorCode(classData);
    }
}

} // namespace reflection
