#include "renderer.hpp"

#include <climits>
#include <format>
#include <memory>

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

} // namespace

struct Renderer::Impl {
    const Model &m_model;
    EventQueue m_event_queue;

    Mode m_mode{ Mode::NORMAL };

    int m_selected{ 0 };

    std::string m_vimline_buf{};
    int m_cursor_position{ INT_MAX };

    ui::App m_screen{ ui::App::TerminalOutput() };

    // this is only a container, the CatchEvent layer belongs to the parent
    // component set in _init_component()
    ui::Component m_menu{ ui::Container::Vertical({}, &m_selected) };

    // this is only a container, the CatchEvent layer belongs to the parent
    // component set in _init_component()
    ui::Component m_vimline{ ui::Container::Horizontal({}, nullptr) };

    // represents the final component
    ui::Component m_component{ _init_component() };

    ui::Loop m_loop{ &m_screen, m_component };

    Impl(const Model &model, EventQueue event_queue) : m_model(model), m_event_queue(event_queue) {
    }

    auto update_menu() -> void {
        m_menu->DetachAllChildren();

        const auto &files = m_model.files;
        for (const auto &f : files) {
            m_menu->Add(_get_entry(f));
        }
        m_selected = m_model.to_select;
    }

  private:
    static auto _get_entry(const FileNode &f) -> ui::Component {
        static const char icons[] = { '-', '>' };

        const auto is_directory = f.is_directory;
        const auto icon = icons[is_directory];
        const auto prefix = [icon](const bool focused) -> std::string_view {
            // NOTE: works only for single-thread and not sure if good design but ehh
            static char ret[] = "00";

            ret[0] = icon;

            if (focused) {
                ret[1] = '>';
            } else {
                ret[1] = '\0';
            }
            return ret;
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

    auto _make_input(std::function<void()> on_enter, ui::Ref<int> cursor_position = INT_MAX) -> ui::Component {
        return ui::Input(&m_vimline_buf, "",
                         ui::InputOption{
                             .transform = [](ui::InputState s) { return s.element; }, // no transform
                             .multiline = false,
                             .on_enter = on_enter,
                             .cursor_position = cursor_position,
                         });
    }
    auto _make_input(ui::InputOption option) -> ui::Component {
        return ui::Input(&m_vimline_buf, "", option);
    }

    auto _set_vimline(ui::Component input, std::string_view input_type) -> void {
        m_vimline->Add(ui::Renderer(input, [input, input_type]() {
            return ui::hbox({
                ui::text(input_type),
                ui::separator(),
                input->Render(),
            });
        }));
        input->TakeFocus();
    }

    auto _set_mode(Mode m) -> void {
        const auto set_normal = [this]() { return _set_mode(Mode::NORMAL); };
        m_mode = m;

        m_vimline->DetachAllChildren();
        switch (m_mode) {
            case Mode::NORMAL: {
                m_vimline->Add(ui::Renderer([]() {
                    return ui::hbox({
                        ui::text("NORMAL"),
                        ui::separator(),
                    });
                }));
                m_menu->TakeFocus();
                break;
            }

            case Mode::SEARCH: {
                const ui::Component input = _make_input([this, set_normal]() -> void {
                    // TODO:
                    set_normal();
                });

                m_vimline_buf.clear();
                _set_vimline(input, "SEARCH");
                break;
            }

            case Mode::CREATE: {
                const ui::Component input = _make_input([this, set_normal]() -> void {
                    m_event_queue->emplace(EVENT::Create_File, EVENT::Data_Create_File{ .name = m_vimline_buf });
                    set_normal();
                });

                m_vimline_buf.clear();
                _set_vimline(input, "CREATE");
                break;
            }

            case Mode::RENAME: {
                const ui::Component input = _make_input(
                    [this, set_normal]() -> void {
                        m_event_queue->emplace(
                            EVENT::Rename,
                            EVENT::Data_Rename{ .path = m_model.files[m_selected].path, .name = m_vimline_buf });
                        set_normal();
                    },
                    &m_cursor_position);

                m_vimline_buf.clear();
                m_vimline_buf = m_model.files[m_selected].path.filename().string();
                m_cursor_position = INT_MAX;
                _set_vimline(input, "RENAME");
                break;
            }
        }
    };

    auto _init_component() -> ui::Component {
        this->update_menu();
        _set_mode(Mode::NORMAL);

        auto menu = ui::CatchEvent(m_menu, [this](ftxui::Event ev) {
            switch (m_mode) {
                case Mode::NORMAL:
                    if (ev == ftxui::Event::q || ev == ftxui::Event::Q) {
                        m_event_queue->push({ EVENT::QUIT, {} });
                        m_screen.Exit();
                        return true;
                    } else if (ev == ui::Event::J) {
                        const auto &files = m_model.files;
                        const auto &to_open = files[m_selected].path;
                        m_event_queue->emplace(EVENT::Open, EVENT::Data_OpenFolder{ .path = to_open });
                        return true;
                    } else if (ev == ui::Event::K) {
                        m_event_queue->emplace(EVENT::Open_Parent_Folder, nullptr);
                        return true;
                    } else if (ev == ui::Event::Character("x")) {
                        /*
                        const auto &files = rdata->model.files;
                        const auto &to_delete = files[rdata->selected].path;
                        m_event_queue->emplace(EVENT::Delete, EVENT::Data_Delete{ .path = to_delete });
                        return true;
                        */
                    } else if (ev == ui::Event::Character('/')) {
                        _set_mode(Mode::SEARCH);
                        return true;
                    } else if (ev == ui::Event::o || ev == ui::Event::O) {
                        _set_mode(Mode::CREATE);
                        return true;
                    } else if (ev == ui::Event::r || ev == ui::Event::R) {
                        _set_mode(Mode::RENAME);
                        return true;
                    }
                default:
                    return false;
            }

            return false;
        });

        auto vimline = ui::CatchEvent(m_vimline, [this](ui::Event ev) {
            if (ev == ui::Event::Escape) {
                _set_mode(Mode::NORMAL);
                return true;
            }
            switch (m_mode) {
                // TODO: stuff
                default: {
                }
            }

            return false;
        });

        auto final_menu = ui::Renderer(menu, [this, menu]() {
            return ui::vbox({
                window(ui::text(m_model.parent_path.string()) | ui::bold,
                       menu->Render() | ui::vscroll_indicator | ui::frame | ui::size(ui::HEIGHT, ui::LESS_THAN, 20)),
            });
        });
        auto final_vimline = ui::Renderer(vimline, [vimline]() {
            return ui::vbox({
                vimline->Render() | ui::border,
            });
        });

        return ui::Container::Vertical({ final_menu, final_vimline });
    }
};

Renderer::Renderer(const Model &m, EventQueue event_queue) : pImpl(std::make_unique<Renderer::Impl>(m, event_queue)) {
}

Renderer::~Renderer() {
}

auto Renderer::render() -> void {
    auto &impl = this->pImpl;
    const auto &model = impl->m_model;
    if (model.updated) {
        impl->update_menu();
        impl->m_screen.RequestAnimationFrame();
    }
    impl->m_loop.RunOnceBlocking();
}
