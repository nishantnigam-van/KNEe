#include "renderer.hpp"

#include "event.hpp"
#include "ftxui/ftxui.hpp"
#include <filesystem>
#include <format>
#include <memory>
#include <queue>

namespace ui = ftxui;
namespace fs = std::filesystem;

namespace {
struct _Data {
    const Model &model;
    int selected{ 0 };

    _Data(const Model &model) : model(model) {
    }

    void update(const ui::Component *pmenu) {
        auto menu = *pmenu;
        menu->DetachAllChildren();

        const auto &files = model.files;

        for (const auto &f : files) {
            auto is_directory = f.is_directory;
            menu->Add(ui::MenuEntry(std::format("{:10.2f}K {}", (float)f.size / 1000, f.path.filename().string()),
                                    ui::MenuEntryOption{ .transform = [is_directory](ui::EntryState state) {
                                        state.label = std::format("{} {}",
                                                                  state.focused ? (is_directory ? ">>" : "->")
                                                                                : (is_directory ? "> " : "- "),
                                                                  state.label);
                                        ui::Element e = ui::text(state.label);

                                        if (is_directory) {
                                            e = e | color(ui::Color::LightSkyBlue1);
                                        }

                                        if (state.focused) {
                                            e |= ui::bgcolor(ui::Color::RGBA(06, 15, 20, 255));
                                            e |= ui::bold;
                                        }

                                        return e;
                                    } }));
        }

        selected = model.to_select;
    }
};

ui::Component init_component(_Data *rdata, ui::App *screen, ui::Component *pmenu, std::queue<Event> *event_queue) {
    rdata->update(pmenu);

    auto menu = *pmenu;

    menu |= ui::CatchEvent([rdata, screen, event_queue](ftxui::Event ev) {
        if (ev == ftxui::Event::q || ev == ftxui::Event::Q) {
            event_queue->push({ EVENT::QUIT, {} });
            screen->Exit();
            return true;
        } else if (ev == ui::Event::J) {
            const auto &files = rdata->model.files;
            const auto &to_open = files[rdata->selected].path;
            if (fs::is_directory(to_open))
                event_queue->push({ EVENT::Open_Folder, EVENT::Data_OpenFolder{ .path = to_open } });
            return true;
        } else if (ev == ui::Event::K) {
            const auto &parent_path = rdata->model.parent_path;
            event_queue->emplace(EVENT::Open_Parent_Folder, nullptr);
            return true;
        }

        return false;
    });

    return ftxui::Renderer(menu, [rdata, menu] {
        return ui::vbox({
            window(ui::text(rdata->model.parent_path.string()) | ui::bold, menu->Render()),
        });
    });
}
} // namespace

struct Renderer::Impl {
    _Data m_rdata;

    ui::App m_screen;
    ui::Component m_menu;
    ui::Component m_component; // represents the final component
    ui::Loop m_loop;

    Impl(_Data &&rdata, ui::App &&screen, std::queue<Event> *event_queue)
        : m_rdata(std::move(rdata)),
          m_screen(std::move(screen)),
          m_menu(ui::Container::Vertical({}, &m_rdata.selected)),
          m_component(init_component(&m_rdata, &m_screen, &m_menu, event_queue)),
          m_loop(&m_screen, m_component) {
    }
};

Renderer::Renderer(const Model &m, std::queue<Event> *event_queue)
    : pImpl(std::make_unique<Renderer::Impl>(_Data(m), ui::App::TerminalOutput(), event_queue)) {
}

Renderer::~Renderer() {
}

void Renderer::render() {
    auto &impl = this->pImpl;
    if (impl->m_rdata.model.updated) {
        impl->m_rdata.update(&impl->m_menu);
        impl->m_screen.RequestAnimationFrame();
    }
    impl->m_loop.RunOnce();
}
