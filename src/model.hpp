#include "file.hpp"
#include <vector>
#ifndef MODEL_H
#define MODEL_H

enum class Mode {
    NORMAL,
    SEARCH,
};

struct Model {
    Mode mode{};
    std::filesystem::path parent_path; // needed cause in some cases
    std::vector<FileNode> files;
};

#endif
