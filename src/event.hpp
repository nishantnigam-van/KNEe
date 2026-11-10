#ifndef EVENT_H
#define EVENT_H

#include <filesystem>
#include <variant>

namespace EVENT {
enum ENUM {
    QUIT,

    // Input-ish
    Open,
    Open_Parent_Folder, // means goes back a directory
    Create_File, // just pass is directory to this
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

struct Data_Create_File { // handle this
    std::string name;
    std::filesystem::path path; // you can chose not to give this // if not given creates in parent directory
    bool is_directory;
};

struct Data_Delete {
    std::filesystem::path path;
};

} // namespace EVENT

struct Event {
    EVENT::ENUM ev;
    std::
        variant<std::nullptr_t, EVENT::Data_OpenFolder, EVENT::Data_Rename, EVENT::Data_Delete, EVENT::Data_Create_File>
            data;
};

#endif
