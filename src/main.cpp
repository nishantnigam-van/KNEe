#include "file_handler.hpp"
#include "renderer.hpp"

int main() {
    Model model;

    create_default(model); // to create a root and update info with the current directory
    // call this fxt to perform change needed
    // handle_action(Model &model, const Event &event);
    Renderer rd(model);

    rd.render();

    return 0;
}
