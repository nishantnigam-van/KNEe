#include "input_manager.hpp"
#include "file_handler.hpp"

InputManager::InputManager(Model &model, std::queue<Event> &event_queue) : m_event_queue(event_queue), m_model(model) {
}

bool InputManager::handle() {
    while (!this->m_event_queue.empty()) {
        auto ev = m_event_queue.back();
        m_event_queue.pop();
        handle_action(this->m_model, ev);
    }

    return true;
}
