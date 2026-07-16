#include "compile_commands.h"
#include "json.hpp"
#include <fstream>
#include <iostream>

namespace reflection
{
using json = nlohmann::json;
std::vector<std::string> get_args(const char* json_path)
{
    std::ifstream file(json_path, std::ios::binary);
    std::vector<std::string> args_storage;
    if(!file.is_open()) {
        std::cerr << "Failed to open file." << std::endl;
        return args_storage;
    }

    json data;
    file >> data;
    for(const auto& entry: data) {
        if(entry.contains("file")) {
            std::string file_path = entry["file"].get<std::string>();
            if(!file_path.ends_with(".cpp")
               && !file_path.ends_with(".cxx")
               && !file_path.ends_with(".cc")) {
                continue;
            }
            if(entry.contains("arguments")) {
                bool prev_flag = false;
                auto&& arguments = entry["arguments"];
                for(size_t i = 0; i < arguments.size(); ++i) {
                    std::string arg_str = arguments[i].get<std::string>();
                    if(arg_str.starts_with("--driver-mode")) {
                        continue;
                    }
                    if(arg_str.starts_with('/') || arg_str.starts_with('-')) {
                        prev_flag = true;
                    } else if(!prev_flag) {
                        continue;
                    } else {
                        prev_flag = false;
                    }
                    args_storage.push_back(arg_str);
                }
                break;
            }
        }
    }
    return args_storage;
}
} // namespace reflection
