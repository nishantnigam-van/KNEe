#pragma once
#include <cstdint>
#include <string>

struct FileNode {
    std::string name;
    std::string path;
    bool is_directory = false; // is folder?
    std::uintmax_t size = 0;
};
