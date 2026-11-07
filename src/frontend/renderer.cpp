#include "renderer.hpp"

#include "event.hpp"
#include "ftxui/ftxui.hpp"
#include <memory>
#include <queue>

#include "file.hpp"

namespace ui = ftxui;

struct Renderer::Impl {
    const Model &m_model;
    std::queue<Event> event_queue{};

    ui::App app;
    ui::Component component; // represents the final component
    int selected; // current selected entry in list of files

    Impl(const Model &m) : m_model(m), app(ui::App::TerminalOutput()) {
    }
};

Renderer::Renderer(const Model &m) : pImpl(std::make_unique<Impl>(m)) {
    auto &impl = this->pImpl;

    auto &parent_path = impl->m_model.parent_path;
    auto &files = impl->m_model.files;
    std::vector<std::string> entries;
    entries.reserve(files.size());
    for (const auto &f : files) {
        entries.emplace_back(f.path.filename().string());
    }

    auto menu = ui::Menu(&entries, &impl->selected);
    menu |= ui::CatchEvent([&](ftxui::Event ev) {
        if (ev == ftxui::Event::q || ev == ftxui::Event::Q) {
            impl->app.Exit();
            return true;
        } else if (ev == ui::Event::J) {
            impl->event_queue.emplace(EVENT::Open_Folder, EVENT::Data_OpenFolder{ .path = files[impl->selected].path });
        } else if (ev == ui::Event::K) {
            impl->event_queue.emplace(EVENT::Open_Folder, EVENT::Data_OpenFolder{ .path = files[impl->selected].path });
        }

        return false;
    });

    auto component = ftxui::Renderer(menu, [&] {
        return ui::vbox({
            window(ui::text(parent_path.string()) | ui::bold, menu->Render()),
        });
    });
    impl->component = component;
    impl->app.Loop(component);
}

Renderer::~Renderer() {
}

std::queue<Event> &Renderer::event_queue() {
    auto &impl = this->pImpl;
    return impl->event_queue;
};

void Renderer::render() {
    auto &impl = this->pImpl;
}
