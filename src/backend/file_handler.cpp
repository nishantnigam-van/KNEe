#include "file_handler.hpp"
#include "event.hpp"

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

void action_open_folder(Model &model, const Event &event) {
    auto *data = static_cast<EVENT::Data_OpenFolder *>(event.data); // kinda convert pointer to that struct
    auto path = data->path; // in the struct its in variable named path
    if (!fs::is_directory(path)) {
        // handle_opening a file
    } else {
        model.files.clear();
        for (auto const &entry : fs::directory_iterator(path)) {
            auto is_directory = fs::is_directory(entry.path());
            model.files.emplace_back(entry.path(), is_directory, is_directory ? 0 : fs::file_size(entry.path()));
        }
    }
}

void create_default(Model &model) {
    auto path = fs::current_path();
    for (auto const &entry : fs::directory_iterator(path)) {
        auto is_directory = fs::is_directory(entry.path());
        model.files.emplace_back(entry.path(), is_directory, is_directory ? 0 : fs::file_size(entry.path()));
    }
}

void handle_action(Model &model, const Event &event) {
    switch (event.ev) {
        case EVENT::Open_Folder:
            action_open_folder(model, event);
            break;
        default:
            break;
    }
}
