#include "callaction.hpp"

// void action_open_openfolder(vector<FileNode> &root) {
//   auto *path = static_cast<EVENT::Data_OpenFolder *>(event.data);
//
//   root->is_directory = fs::is_directory(path);
//   root->path = path;
//
// }

void create_default(Model &model) {
    auto path = fs::current_path();
    for (auto const &entry : fs::directory_iterator(path)) {
        FileNode newfile = { entry.path().filename(), entry.path(), fs::is_directory(entry.path()),
                             fs::is_directory(entry.path()) ? fs::file_size(entry.path()) : 0 };
        model.files.push_back(newfile);
    }
}

// void handle_action(Model &model,const Event &event) {
//   switch (event.ev):
//   case EVENT::Open_Folder :
//     action_open_folder(FileNode & file);
//     default : std::cout << "Something bullshit action requested!\n";
// }
