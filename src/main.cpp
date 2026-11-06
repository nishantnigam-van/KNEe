#include "renderer.hpp"

int main() {
    Model model;
    Renderer rd(model);

    rd.render();

    return 0;
}
