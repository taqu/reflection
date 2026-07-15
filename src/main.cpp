#include <iostream>
#include <vector>
#include <clang-c/CXCompilationDatabase.h>
#include <clang-c/Index.h>
#include "compile_commands.h"

int main(void)
{
        std::vector<std::string> args_storage = ast::get_args("./build/compile_commands.json");
        std::vector<const char*> clang_args;
        clang_args.reserve(args_storage.size());
        for(size_t i=0; i<args_storage.size(); ++i){
            clang_args.push_back(args_storage[i].c_str());
        }

        CXIndex index = clang_createIndex(0, 0);
        CXTranslationUnit tu;
        
        enum CXErrorCode parse_err = clang_parseTranslationUnit2(
            index, "./include/compile_commands.h",
            clang_args.data(), clang_args.size(),
            nullptr, 0, CXTranslationUnit_None, &tu
        );

        if (parse_err == CXError_Success) {
            std::cout << "パース成功" << std::endl;
            clang_disposeTranslationUnit(tu);
        }
        clang_disposeIndex(index);
    return 0;
}
