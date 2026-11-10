// note :1. add create and perform fxn if multiple paths are given
// 2. create_directory can be use to copy (i guess)
// 3. add what more fxns you want........(i cant remember)
#include "file_handler.hpp"
#include "event.hpp"
#include "file.hpp"
#include "file_opener.hpp"

#include <filesystem>
#include <string>
#include <system_error>
#include <vector>

namespace fs = std::filesystem;

namespace {

void update_model(Model &model, const auto &path) {
    model.files.clear();
    for (auto const &entry : fs::directory_iterator(path)) {
        auto is_directory = fs::is_directory(entry.path());
        model.files.emplace_back(entry.path(), is_directory, is_directory ? 0 : fs::file_size(entry.path()));
    }
}

void sort_files(Model &model) {
    auto &files = model.files;
    std::sort(files.begin(), files.end(), [](FileNode x, FileNode y) -> int {
        if (x.is_directory && y.is_directory) {
            return x.path < y.path;
        } else if (x.is_directory) {
            return true;
        } else if (y.is_directory) {
            return false;
        } else {
            return x.path < y.path;
        }
    });
}

void action_open_folder(Model &model, const Event &event) {
    const auto &data = get<EVENT::Data_OpenFolder>(event.data);
    auto &path = data.path; // in the struct its in variable named path
    model.parent_path = path;
    if (!fs::is_directory(path)) {
        // handle_opening a file
        bool opened = openfile(path);
        if (!opened) model.current_error = "Couldn't open the file!"; // might think some better way to solve this!
    } else {
        update_model(model, path);
        sort_files(model);
    }
}

void action_open_parent_folder(Model &model, const Event &event) {
    const auto old_path = model.parent_path;
    model.parent_path = model.parent_path.parent_path();

    update_model(model, model.parent_path);
    sort_files(model);

    int i = 0;
    for (auto const &entry : model.files) {
        if (entry.path == old_path) {
            model.to_select = i;
            break;
        } else {
            i++;
        }
    }
}

void action_delete(Model &model, const Event &event) {
    const auto &data = std::get<EVENT::Data_Delete>(event.data); // kinda convert pointer to that struct
    auto &path = data.path;
    std::error_code error;
    if (fs::remove_all(path, error)) { // this one return uintmax_t that is the number of files deleted
        update_model(model, model.parent_path);
        sort_files(model);
    }
    if (error) model.current_error = "Error Deleting files " + error.message();
}

void action_rename(Model &model, const Event &event) {
    const auto &data = std::get<EVENT::Data_Rename>(event.data); // kinda convert pointer to that struct
    auto &path = data.path;
    // rename( const std::filesystem::path& old_p,
    //          const std::filesystem::path& new_p );
    std::error_code error;
    fs::rename(path, model.parent_path / data.name, error);
    if (error)
        model.current_error = "Error renaming files " + error.message();
    else {
        update_model(model, model.parent_path);
        sort_files(model);
    }
}

void action_create_File(Model &model, const Event &event) {
    const auto &data = std::get<EVENT::Data_Create_File>(event.data); // kinda convert pointer to that struct
    std::error_code error;
    if (data.is_directory) {
        if (data.name.length()) {
            // if(fs::create_directory(data.path, const path &attributes, error_code &ec)) // add for multiple path
            // passed
        } else {
            // add case for no name is passed and asked to create a directory
        }
    } else {
        // add execution for creating file;
    }
}

} // namespace

void create_default(Model &model) {
    model.parent_path = fs::current_path();

    update_model(model, model.parent_path);

    model.updated = true;

    sort_files(model);
}

void handle_action(Model &model, const Event &event) {
    model.current_error.clear();
    switch (event.ev) {
        case EVENT::Open:
            action_open_folder(model, event);
            break;
        case EVENT::Open_Parent_Folder:
            action_open_parent_folder(model, event);
            break;
        case EVENT::Delete:
            action_delete(model, event);
            break;
        case EVENT::Rename:
            action_rename(model, event);
            break;
        case EVENT::Create_File:
            // action_create_File(model,event);
            break;
        default:
            break;
    }
}
