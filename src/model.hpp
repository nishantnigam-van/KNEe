#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <cstdint>
#include <filesystem>

struct FileNode {
    std::filesystem::path path;
    bool is_directory = false; // is folder?
    std::uintmax_t size = 0;
};

struct Model {
    bool updated{ false };

    std::filesystem::path parent_path; // needed cause in some cases
    std::vector<FileNode> files;
    int to_select{ 0 }; // not actual position of selection, only during re-modelling for going up a directory

    std::string current_error;
};

#endif
