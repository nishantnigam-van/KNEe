#include "file_handler.hpp"
#include "input_manager.hpp"
#include "renderer.hpp"

int main() {
    Model model;

    create_default(model); // to create a root and update info with the current directory
    // call this fxt to perform change needed
    // handle_action(Model &model, const Event &event);

    InputManager inp(model);
    Renderer rd(model, inp.event_queue());

    auto run = true;
    while (run) {
        rd.render();

        // reset Model defaults
        inp.reset_defaults();

        run = inp.handle();
    }

    return 0;
}
