#pragma once
#include "event.hpp"
#include "file.hpp"
#include "model.hpp"
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

// void handle_action(FileNode &file, const Event &e);
void create_default(Model &model);
