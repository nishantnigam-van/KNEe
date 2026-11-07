#ifndef EVENT_H
#define EVENT_H

#include <filesystem>
#include <variant>

namespace EVENT {
enum ENUM {
    QUIT,

    // Input-ish
    Open_Folder,
    Open_Parent_Folder, // means goes back a directory
    Create_Directory,
    Create_File,
    Rename,
    Delete,
};

struct Data_OpenFolder {
    std::filesystem::path path;
};

struct Data_Rename {
    std::filesystem::path path;
    std::string name;
};

struct Data_Delete {
    std::filesystem::path path;
};

} // namespace EVENT

struct Event {
    EVENT::ENUM ev;
    std::variant<EVENT::Data_OpenFolder, EVENT::Data_Rename, EVENT::Data_Delete> data;
};

#endif
