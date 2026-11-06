#include "renderer.hpp"

#include "ftxui/ftxui.hpp"
#include <memory>

#include "file.hpp"

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

    auto &files = impl->m_model.files;
    std::vector<std::string> entries;
    entries.reserve(files.size());
    for (const auto &f : files) {
        entries.emplace_back(f.path.filename().string());
    }

    int selected = 0;

    // Create a menu component (can also be decorated with | (pipe) operator)
    auto menu = Menu(&entries, &selected);

    auto component = ftxui::Renderer(menu, [&] {
        return vbox({
            window(text("path ") | bold, menu->Render()),
        });
    });

    impl->component = component;
    impl->app.Loop(component);
}

Renderer::~Renderer() {
}

void Renderer::render() {
    auto &impl = this->pImpl;
}
