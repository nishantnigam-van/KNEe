#ifndef EVENT_H
#define EVENT_H

#include <filesystem>

namespace EVENT {
enum ENUM {
    // Input-ish
    Open_Folder,
    Open_Parent_Folder, // means goes back a directory
};

struct Data_OpenFolder {
    std::filesystem::path path;
};

} // namespace EVENT

struct Event {
    EVENT::ENUM ev;
    void *data;
};

#endif
