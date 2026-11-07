#include "input_manager.hpp"
#include "file_handler.hpp"

#include <fstream>
#include <iostream>
#include <string>

static void logToFile(const std::string &message) {
    std::ofstream logfile("application.log", std::ios::app);

    if (!logfile.is_open()) {
        exit(1);
        std::cerr << "Failed to open log file\n";
        return;
    }

    logfile << message << '\n';
}

InputManager::InputManager(Model &model, std::queue<Event> &event_queue) : m_event_queue(event_queue), m_model(model) {
}

bool InputManager::handle() {
    while (!this->m_event_queue.empty()) {
        logToFile("ev");
        auto ev = m_event_queue.back();
        m_event_queue.pop();
        handle_action(this->m_model, ev);
        if (ev.ev == EVENT::QUIT) {
            return false;
        }
    }

    return true;
}
