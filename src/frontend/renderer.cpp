#include "renderer.hpp"

#include "ftxui/ftxui.hpp"
#include <memory>

using namespace ftxui;

struct Renderer::Impl {
    const Model &m_model;
    App app;
    Component component;

    Impl(const Model &m) : m_model(m), app(App::TerminalOutput()) {
    }
};

Renderer::Renderer(const Model &m) : pImpl(std::make_unique<Impl>(m)) {
    auto &impl = this->pImpl;

    std::vector<std::string> entries = {
        "Entry 1",
        "Entry 2",
        "Entry 3",
    };
    int selected = 0;

    // Create a menu component
    auto menu = Menu(&entries, &selected);

    // You can decorate components using the pipe operator.
    auto component = menu | border;

    impl->component = component;
    impl->app.Loop(component);
}

Renderer::~Renderer() {
}

void Renderer::render() {
    auto &impl = this->pImpl;
}
