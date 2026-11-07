#include "file_handler.hpp"
#include "renderer.hpp"

int main() {
    Model model;

    create_default(model); // to create a root and update info with the current directory

    Renderer rd(model);

    rd.render();

    return 0;
}
