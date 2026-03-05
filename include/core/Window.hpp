#ifndef CORE_WINDOW_HPP
#define CORE_WINDOW_HPP
#pragma once

#include <array>
#include <cstdint>
#include <functional>

#include "core/Event.hpp"

namespace gpudsp::core {

class Window final {
public:

    using EventCallback = std::function<void(Event*)>;
    static const EventCallback s_default_event_callback;

    struct WindowParameters {

        /**
         * This looks weird, but both clang and gcc 
         * will throw errors when using the default
         * constructor. It's a known bug.
         */
        inline static WindowParameters Default(void) { return {}; }

        EventCallback event_callback = s_default_event_callback;
        const char* const title = "GPU DSP";
        std::array<int, 2> size = { 800, 600 };
    };

    static Window* getInstance(const WindowParameters& parameters = WindowParameters::Default());

    void setEventCallback(const EventCallback& event_callback) { m_parameters.event_callback = event_callback; }

    void update(void);
    void makeCurrent(void);
    bool shouldClose(void);

    inline void raiseEvent(Event* event) { m_parameters.event_callback(event); }

    inline std::array<int, 2> getSize(void) const { return m_parameters.size; }
    inline int getCursorActiveButton(void) const { return m_cursor.active_button; }
    inline int getCursorMods(void) const { return m_cursor.mods; }
    inline std::array<double, 2> getCursorPosition(void) const { return m_cursor.position; }

    inline void setSize(const std::array<int, 2>& size) { m_parameters.size = size; }
    inline void setCursorActiveButton(int button) { m_cursor.active_button = button; }
    inline void setCursorMods(int mods) { m_cursor.mods = mods; }
    inline void setCursorPosition(const std::array<double, 2>& position) { m_cursor.position = position; }

private:

    struct Cursor {
        std::array<double, 2> position = { 0.0, 0.0 };
        int active_button = -1;
        int mods = -1;
    };

    Window(const WindowParameters& parameters);
    ~Window(void);

    static Window* s_instance;

    void* m_glfw_window;
    Cursor m_cursor;
    WindowParameters m_parameters;

};

}

#endif