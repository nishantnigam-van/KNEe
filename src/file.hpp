#ifndef FILE_H
#define FILE_H

#include <cstdint>
#include <filesystem>

struct FileNode {
    std::filesystem::path path;
    bool is_directory = false; // is folder?
    std::uintmax_t size = 0;
};

#endif
