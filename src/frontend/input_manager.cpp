#include "input_manager.hpp"

#include "file_handler.hpp"

InputManager::InputManager(Model &model) : m_model(model) {
}

auto InputManager::handle() -> bool {
    while (!m_event_queue->empty()) {
        auto ev = m_event_queue->back();
        m_event_queue->pop();

        handle_action(m_model, ev);

        if (ev.ev == EVENT::QUIT) {
            return false;
        } else {
            m_model.updated = true;
        }
    }

    return true;
}

auto InputManager::reset_defaults() noexcept -> void {
    if (m_model.updated == 1) {
        m_model.updated = 0;
    }
    m_model.to_select = 0;
}
