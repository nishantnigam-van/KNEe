#ifndef MODEL_H
#define MODEL_H

enum class Mode {
    NORMAL,
    SEARCH,
};

struct Model {
    Mode mode{};
};

#endif
