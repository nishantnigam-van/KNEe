#include "file.hpp"
#include <vector>
#ifndef MODEL_H
#define MODEL_H

enum class Mode {
    NORMAL,
    SEARCH,
};

struct Model {
    bool updated{ false };

    Mode mode{};
    std::filesystem::path parent_path; // needed cause in some cases
    std::vector<FileNode> files;
    int to_select{ 0 }; // not actual position of selection, only during re-modelling for going up a directory

    std::string current_error;
};

#endif
