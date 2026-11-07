#include "file_handler.hpp"
#include "event.hpp"
#include "file_opener.hpp"

#include <filesystem>
#include <iostream>
#include <system_error>
#include <vector>

namespace fs = std::filesystem;

void create_default(Model &model) {
    auto path = fs::current_path();
    model.parent_path = path;
    for (auto const &entry : fs::directory_iterator(path)) {
        auto is_directory = fs::is_directory(entry.path());
        model.files.emplace_back(entry.path(), is_directory, is_directory ? 0 : fs::file_size(entry.path()));
    }
}

void action_open_folder(Model &model, const Event &event) {
    const auto &data = get<EVENT::Data_OpenFolder>(event.data); // kinda convert pointer to that struct
    auto &path = data.path; // in the struct its in variable named path
    model.parent_path = path;
    if (!fs::is_directory(path)) {
        // handle_opening a file
        bool opened = openfile(path);
        if (!opened) std::cout << "Couldn't Open the file!"; // might think some better way to solve this!
    } else {
        model.files.clear();
        for (auto const &entry : fs::directory_iterator(path)) {
            auto is_directory = fs::is_directory(entry.path());
            model.files.emplace_back(entry.path(), is_directory, is_directory ? 0 : fs::file_size(entry.path()));
        }
    }
}
void action_open_parent_folder(Model &model, const Event &event) {
    auto new_parent_path = model.parent_path.parent_path();
    model.files.clear();
    for (auto const &entry : fs::directory_iterator(new_parent_path)) {
        auto is_directory = fs::is_directory(entry.path());
        model.files.emplace_back(entry.path(), is_directory, is_directory ? 0 : fs::file_size(entry.path()));
    }
}

void action_delete_file(Model &model, const Event &event) {
    const auto &data = std::get<EVENT::Data_Delete>(event.data); // kinda convert pointer to that struct
    auto &path = data.path;
    std::error_code error;
    if (fs::remove_all(path, error)) { // this one return uintmax_t that is the number of files deleted
        for (auto const &entry : fs::directory_iterator(model.parent_path)) {
            auto is_directory = fs::is_directory(entry.path());
            model.files.emplace_back(entry.path(), is_directory, is_directory ? 0 : fs::file_size(entry.path()));
        }
    }
    if (error) std::cout << "Error Deleting files" << error.message();
}

void action_rename(Model &model, const Event &event) {
    const auto &data = std::get<EVENT::Data_Rename>(event.data); // kinda convert pointer to that struct
    auto &path = data.path;
    // rename( const std::filesystem::path& old_p,
    //          const std::filesystem::path& new_p );
    std::error_code error;
    fs::rename(path, model.parent_path / data.name, error);
    if (error) std::cout << "Error Renaming the file!" << error.message();
}

void handle_action(Model &model, const Event &event) {
    switch (event.ev) {
        case EVENT::Open_Folder:
            action_open_folder(model, event);
            break;
        case EVENT::Open_Parent_Folder:
            action_open_parent_folder(model, event);
            break;
        case EVENT::Delete:
            // action_delete_file(model,event);
            break;
        case EVENT::Rename:
            // action_rename_file(model,event);
            break;
        case EVENT::Create_Directory:
            // action_create_directory(model,event);
            break;
        case EVENT::Create_File:
            // action_create_file(model,event);
            break;
        default:
            break;
    }
}
