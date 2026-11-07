#ifndef EVENT_H
#define EVENT_H

#include <filesystem>

namespace EVENT {
enum ENUM {
    // Input-ish
    Open_Folder,
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
