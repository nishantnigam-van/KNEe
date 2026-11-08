#include "input_manager.hpp"
#include "file_handler.hpp"

#include <queue>

InputManager::InputManager(Model &model) : m_model(model) {
}

std::queue<Event> *InputManager::event_queue() {
    return &m_event_queue;
};

bool InputManager::handle() {
    while (!this->m_event_queue.empty()) {
        auto ev = m_event_queue.back();
        m_event_queue.pop();

        handle_action(this->m_model, ev);

        if (ev.ev == EVENT::QUIT) {
            return false;
        } else {
            m_model.updated = true;
        }
    }

    return true;
}

void InputManager::reset_defaults() {
    if (m_model.updated == 1) {
        m_model.updated = 0;
    }
    m_model.to_select = 0;
}
