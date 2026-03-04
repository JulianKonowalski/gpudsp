#pragma once

#include <array>
#include <string>

namespace gpudsp::core {

/*----------------------------------------------------------------------------*/

class Event {
public:

    enum class Type {
        WindowClosed,
        WindowResized,
        MouseMoved,
        MousePressed,
        MouseReleased,
        MouseDragged,
        MouseScrolled,
        ButtonPressed,
        ButtonReleased
    };

    Event(void) : m_handled(false) {}

    virtual ~Event(void) {};
    virtual Event::Type eventType(void) const = 0;
    virtual std::string toString(void) const = 0;

    inline void setHandled(void) { m_handled = true; }
    inline bool isHandled(void) const { return m_handled; }

private:

    bool m_handled;

};

/*----------------------------------------------------------------------------*/

class WindowResizeEvent : public Event {
public:

    WindowResizeEvent(const std::array<int, 2>& size) : m_size(size) {}
    ~WindowResizeEvent(void) override {}
    
    inline std::array<int, 2> size(void) const { return m_size;}

    Event::Type eventType(void) const override { return Event::Type::WindowResized; }
    std::string toString(void) const override {
        return std::string("[WindowResizeEvent]")
            + std::string(" width: ") + std::to_string(m_size[0])
            + std::string(" height: ") + std::to_string(m_size[1]);
    }

private:

    std::array<int, 2> m_size;

};

/*----------------------------------------------------------------------------*/

class WindowCloseEvent : public Event {
public:

    WindowCloseEvent(void) {}
    ~WindowCloseEvent(void) override {}
    
    Event::Type eventType(void) const override { return Event::Type::WindowClosed; }
    std::string toString(void) const override { return std::string("[WindowCloseEvent]"); }

};

/*----------------------------------------------------------------------------*/

class MouseMoveEvent : public Event {
public:

    using Point = std::array<double, 2>;

    MouseMoveEvent(const Point& position) : m_position(position) {}
    ~MouseMoveEvent(void) override {}
    
    inline Point position(void) const { return m_position; }

    Event::Type eventType(void) const override { return Event::Type::MouseMoved; }
    std::string toString(void) const override {
        return std::string("[MouseMoveEvent]")
            + std::string(" x: ") + std::to_string(m_position[0])
            + std::string(" y: ") + std::to_string(m_position[1]);
    }

private:

    Point m_position;

};

/*----------------------------------------------------------------------------*/

class MousePressEvent : public Event {
public:

    using Point = std::array<double, 2>;

    MousePressEvent(const Point& position, int button, int mods) 
        : m_position(position), m_button(button), m_mods(mods) {}
    ~MousePressEvent(void) override {}

    inline Point position(void) const { return m_position; }
    inline int button(void) const { return m_button; }
    inline int mods(void) const { return m_mods; }

    Event::Type eventType(void) const override { return Event::Type::MousePressed; }
    std::string toString(void) const override {
        return std::string("[MousePressEvent]")
            + std::string(" x: ") + std::to_string(m_position[0])
            + std::string(" y: ") + std::to_string(m_position[1])
            + std::string(" button: ") + std::to_string(m_button)
            + std::string(" mods: ") + std::to_string(m_mods);
    }

private:

    Point m_position;
    int m_button;
    int m_mods;

};

/*----------------------------------------------------------------------------*/

class MouseReleaseEvent : public Event {
public:

    using Point = std::array<double, 2>;

    MouseReleaseEvent(const Point& position, int button, int mods)
        : m_position(position), m_button(button), m_mods(mods) {}
    ~MouseReleaseEvent(void) override {}

    inline Point position(void) const { return m_position; }
    inline int button(void) const { return m_button; }
    inline int mods(void) const { return m_mods; }

    Event::Type eventType(void) const override { return Event::Type::MouseReleased; }
    std::string toString(void) const override {
        return std::string("[MouseReleaseEvent]")
            + std::string(" x: ") + std::to_string(m_position[0])
            + std::string(" y: ") + std::to_string(m_position[1])
            + std::string(" button: ") + std::to_string(m_button)
            + std::string(" mods: ") + std::to_string(m_mods);
    }

private:

    Point m_position;
    int m_button;
    int m_mods;

};

/*----------------------------------------------------------------------------*/

class MouseDragEvent : public Event {
public:

    using Point = std::array<double, 2>;

    MouseDragEvent(const Point& start_pos, const Point& end_pos, int button, int mods)
        : m_start_pos(start_pos), m_end_pos(end_pos), m_button(button), m_mods(mods) {}
    ~MouseDragEvent(void) override {}

    inline Point start(void) const { return m_start_pos; }
    inline Point end(void) const { return m_end_pos; }
    inline Point distance(void) const { return { m_end_pos[0] - m_start_pos[0], m_end_pos[1] - m_start_pos[1] }; }

    inline int button(void) const { return m_button; }
    inline int mods(void) const { return m_mods; }

    Event::Type eventType(void) const override { return Event::Type::MouseDragged; }
    std::string toString(void) const override { 
        return std::string("[MouseDragEvent]")
            + std::string(" start: ") + std::to_string(m_start_pos[0]) + std::string(" ") + std::to_string(m_start_pos[1])
            + std::string(" end: ") + std::to_string(m_end_pos[0]) + std::string(" ") + std::to_string(m_end_pos[1])
            + std::string(" button: ") + std::to_string(m_button)
            + std::string(" mods: ") + std::to_string(m_mods);
    }

private:

    Point m_start_pos;
    Point m_end_pos;
    int m_button;
    int m_mods;

};

/*----------------------------------------------------------------------------*/

class MouseScrollEvent : public Event {
public:
    
    using Point = std::array<double, 2>;

    MouseScrollEvent(const Point& position, double x_offset, double y_offset)
        : m_position(position), m_x_offset(x_offset), m_y_offset(y_offset) {}
    ~MouseScrollEvent(void) override {}

    inline Point position(void) const { return m_position; }
    inline double xOffset(void) const { return m_x_offset; }
    inline double yOffset(void) const { return m_y_offset; }

    Event::Type eventType(void) const override { return Event::Type::MouseScrolled; }
    std::string toString(void) const override {
        return std::string("[MouseScrollEvent]")
            + std::string(" x: ") + std::to_string(m_position[0])
            + std::string(" y: ") + std::to_string(m_position[1])
            + std::string(" x_offset: ") + std::to_string(m_x_offset)
            + std::string(" y_offset: ") + std::to_string(m_y_offset);
    }

private:

    Point m_position;
    double m_x_offset;
    double m_y_offset;

};

/*----------------------------------------------------------------------------*/

class ButtonPressEvent : public Event {
public:

    ButtonPressEvent(int key, int scancode, int mods)
        : m_key(key), m_scancode(scancode), m_mods(mods) {}
    ~ButtonPressEvent(void) override {}

    inline int key(void) const { return m_key; }
    inline int scancode(void) const { return m_scancode; }
    inline int mods(void) const { return m_mods; }

    Event::Type eventType(void) const override { return Event::Type::ButtonPressed; }
    std::string toString(void) const override {
        return std::string("[ButtonPressEvent]")
            + std::string(" key: ") + std::to_string(m_key)
            + std::string(" scancode: ") + std::to_string(m_scancode)
            + std::string(" mods: ") + std::to_string(m_mods);
    }

private:

    int m_key;
    int m_scancode;
    int m_mods;

};

/*----------------------------------------------------------------------------*/

class ButtonReleaseEvent : public Event {
public:

    ButtonReleaseEvent(int key, int scancode, int mods)
        : m_key(key), m_scancode(scancode), m_mods(mods) {}
    ~ButtonReleaseEvent(void) override {}

    inline int key(void) const { return m_key; }
    inline int scancode(void) const { return m_scancode; }
    inline int mods(void) const { return m_mods; }

    Event::Type eventType(void) const override { return Event::Type::ButtonReleased; }
    std::string toString(void) const override {
        return std::string("[ButtonReleaseEvent]")
            + std::string(" key: ") + std::to_string(m_key)
            + std::string(" scancode: ") + std::to_string(m_scancode)
            + std::string(" mods: ") + std::to_string(m_mods);
    }

private:

    int m_key;
    int m_scancode;
    int m_mods;

};

/*----------------------------------------------------------------------------*/

}