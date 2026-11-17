#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <memory>

#include "event_queue.hpp"
#include "model.hpp"

class InputManager {
  private:
    Model &m_model;
    EventQueue m_event_queue{ std::make_shared<EventQueue::element_type>() };

  public:
    InputManager(Model &m);

    auto event_queue() noexcept {
        return m_event_queue;
    }

    auto handle() -> bool;
    auto reset_defaults() noexcept -> void;
};

#endif
