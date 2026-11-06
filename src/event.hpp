#ifndef EVENT_H
#define EVENT_H

#include <string>

namespace EVENT {
enum ENUM {
    // Input-ish
    Open_Folder,
};

struct Data_OpenFolder {
    std::string path;
};

} // namespace EVENT

struct Event {
    EVENT::ENUM ev;
    void *data;
};

#endif
