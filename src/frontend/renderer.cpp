#include "renderer.hpp"

#include <climits>
#include <filesystem>
#include <format>
#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <memory>
#include <queue>

#include "event.hpp"
#include "ftxui/ftxui.hpp"

namespace ui = ftxui;

namespace {

enum class Mode : std::uint8_t {
    NORMAL,
    SEARCH,
    CREATE,
    RENAME,
};

struct _Data {
  public:
    Mode mode{ Mode::NORMAL };

    const Model &model;
    int selected{ 0 };
    std::string vimline_buf;

    std::queue<Event> *event_queue{ nullptr };

  public:
    _Data(const Model &model, std::queue<Event> *event_queue) : model(model), event_queue(event_queue) {
    }

    void update_menu(ui::Component menu) {
        menu->DetachAllChildren();

        const auto &files = model.files;
        for (const auto &f : files) {
            menu->Add(_get_entry(f));
        }
        selected = model.to_select;
    }

    void set_mode(Mode m, ui::Component menu, ui::Component vimline) {
        this->mode = m;

        vimline->DetachAllChildren();

        switch (this->mode) {
            case Mode::NORMAL: {
                vimline->Add(ui::Renderer([]() {
                    return ui::hbox({
                        ui::text("NORMAL"),
                        ui::separator(),
                    });
                }));
                menu->TakeFocus();
                break;
            }

            case Mode::SEARCH: {
                static ui::Component input =
                    ui::Input(&vimline_buf, "",
                              ui::InputOption{
                                  .transform = [](ui::InputState s) -> ui::Element { return s.element; },
                                  .multiline = false,
                                  .on_enter = [this, menu, vimline]() -> void {
                                      // TODO:
                                      set_mode(Mode::NORMAL, menu, vimline);
                                  },
                              });

                vimline_buf.clear();
                vimline->Add(ui::Renderer(input, []() {
                    return ui::hbox({
                        ui::text("SEARCH"),
                        ui::separator(),
                        input->Render(),
                    });
                }));
                input->TakeFocus();
                break;
            }

            case Mode::CREATE: {
                static ui::Component input =
                    ui::Input(&vimline_buf, "",
                              ui::InputOption{
                                  .transform = [](ui::InputState s) -> ui::Element { return s.element; },
                                  .multiline = false,
                                  .on_enter = [this, menu, vimline]() -> void {
                                      this->event_queue->emplace(EVENT::Create_File, EVENT::Data_Create_File{
                                                                                         .name = vimline_buf,
                                                                                     });
                                      set_mode(Mode::NORMAL, menu, vimline);
                                  },
                              });

                vimline_buf.clear();
                vimline->Add(ui::Renderer(input, []() {
                    return ui::hbox({
                        ui::text("CREATE"),
                        ui::separator(),
                        input->Render(),
                    });
                }));
                input->TakeFocus();
                break;
            }

            case Mode::RENAME: {
                static auto cursor_position = INT_MAX;
                static ui::Component input = ui::Input(
                    &vimline_buf, ui::InputOption{
                                      .transform = [](ui::InputState s) -> ui::Element { return s.element; },
                                      .multiline = false,
                                      .on_enter = [this, menu, vimline]() -> void {
                                          this->event_queue->emplace(EVENT::Rename,
                                                                     EVENT::Data_Rename{
                                                                         .path = this->model.files[this->selected].path,
                                                                         .name = vimline_buf,
                                                                     });
                                          set_mode(Mode::NORMAL, menu, vimline);
                                      },
                                      .cursor_position = &cursor_position,
                                  });

                vimline_buf.clear();
                vimline_buf = this->model.files[this->selected].path.filename().string();
                cursor_position = INT_MAX;
                vimline->Add(ui::Renderer(input, []() {
                    return ui::hbox({
                        ui::text("RENAME"),
                        ui::separator(),
                        input->Render(),
                    });
                }));
                input->TakeFocus();
                break;
            }
        }
    };

  private:
    ui::Component _get_entry(FileNode f) {
        static const std::string icons[] = { "-", ">" };

        const auto is_directory = f.is_directory;
        const auto icon = icons[is_directory];
        const auto prefix = [icon](const bool focused) -> std::string {
            if (focused)
                return icon + '>';
            else
                return icon;
        };
        const auto color = is_directory ? ui::Color::LightSkyBlue1 : ui::Color::RGBA(0, 0, 0, 0);

        return ui::MenuEntry(f.path.filename().string(),
                             ui::MenuEntryOption{ .transform = [prefix, color](ui::EntryState state) -> ui::Element {
                                 state.label = std::format("{}  {}", prefix(state.focused), state.label);
                                 ui::Element e = ui::text(state.label);

                                 e = e | ui::color(color);

                                 if (state.focused) {
                                     e |= ui::bgcolor(ui::Color::RGBA(05, 05, 05, 255));
                                     e |= ui::bold;
                                 }

                                 return e;
                             } });
    }
};

ui::Component init_component(
    _Data *rdata, ui::App *pscreen, ui::Component menu, ui::Component vimline, std::queue<Event> *event_queue) {
    rdata->update_menu(menu);
    rdata->set_mode(Mode::NORMAL, menu, vimline);

    menu |= ui::CatchEvent([rdata, pscreen, menu, vimline, event_queue](ftxui::Event ev) {
        switch (rdata->mode) {
            case Mode::NORMAL:
                if (ev == ftxui::Event::q || ev == ftxui::Event::Q) {
                    event_queue->push({ EVENT::QUIT, {} });
                    pscreen->Exit();
                    return true;
                } else if (ev == ui::Event::J) {
                    const auto &files = rdata->model.files;
                    const auto &to_open = files[rdata->selected].path;
                    event_queue->emplace(EVENT::Open, EVENT::Data_OpenFolder{ .path = to_open });
                    return true;
                } else if (ev == ui::Event::K) {
                    event_queue->emplace(EVENT::Open_Parent_Folder, nullptr);
                    return true;
                } else if (ev == ui::Event::Character("x")) {
                    /*
                    const auto &files = rdata->model.files;
                    const auto &to_delete = files[rdata->selected].path;
                    event_queue->emplace(EVENT::Delete, EVENT::Data_Delete{ .path = to_delete });
                    return true;
                    */
                } else if (ev == ui::Event::Character('/')) {
                    rdata->set_mode(Mode::SEARCH, menu, vimline);
                    return true;
                } else if (ev == ui::Event::o || ev == ui::Event::O) {
                    rdata->set_mode(Mode::CREATE, menu, vimline);
                    return true;
                } else if (ev == ui::Event::r || ev == ui::Event::R) {
                    rdata->set_mode(Mode::RENAME, menu, vimline);
                    return true;
                }
            default:
                return false;
        }

        return false;
    });

    vimline |= ui::CatchEvent([rdata, menu, vimline](ui::Event ev) {
        if (ev == ui::Event::Escape) {
            rdata->set_mode(Mode::NORMAL, menu, vimline);
            return true;
        }
        switch (rdata->mode) {
            // TODO: stuff
            default: {
            }
        }

        return false;
    });

    auto final_menu = ui::Renderer(menu, [rdata, menu]() {
        return ui::vbox({
            window(ui::text(rdata->model.parent_path.string()) | ui::bold, menu->Render()),
        });
    });

    auto final_vimline = ui::Renderer(vimline, [vimline]() {
        return ui::vbox({
            vimline->Render() | ui::border,
        });
    });

    return ui::Container::Vertical({ final_menu, final_vimline });
}
} // namespace

struct Renderer::Impl {
    _Data m_rdata;

    ui::App m_screen;
    ui::Component m_menu;
    ui::Component m_vimline;
    ui::Component m_component; // represents the final component
    ui::Loop m_loop;

    Impl(const Model &model, ui::App &&screen, std::queue<Event> *event_queue)
        : m_rdata(model, event_queue),
          m_screen(std::move(screen)),
          m_menu(ui::Container::Vertical({}, &m_rdata.selected)),
          m_vimline(ui::Container::Horizontal({}, nullptr)),
          m_component(init_component(&m_rdata, &m_screen, m_menu, m_vimline, event_queue)),
          m_loop(&m_screen, m_component) {
    }
};

Renderer::Renderer(const Model &m, std::queue<Event> *event_queue)
    : pImpl(std::make_unique<Renderer::Impl>(m, ui::App::TerminalOutput(), event_queue)) {
}

Renderer::~Renderer() {
}

void Renderer::render() {
    auto &impl = this->pImpl;
    const auto &model = impl->m_rdata.model;
    if (model.updated) {
        impl->m_rdata.update_menu(impl->m_menu);
        impl->m_screen.RequestAnimationFrame();
    }
    impl->m_loop.RunOnce();
}
