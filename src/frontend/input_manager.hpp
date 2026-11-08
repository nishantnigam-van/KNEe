#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "event.hpp"
#include "model.hpp"

#include <queue>

class InputManager {
  public:
    InputManager(Model &m);

    std::queue<Event> *event_queue();
    bool handle();
    void reset_defaults();

  private:
    std::queue<Event> m_event_queue{};
    Model &m_model;
};

#endif
