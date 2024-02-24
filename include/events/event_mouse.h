#pragma once

#include "events/event.h"
#include "utils/vector.h"

enum class MouseButton {
    Unknown,
    Left,
    Middle,
    Right,
};

class EventMouseDown : public Event {
public:
    EventMouseDown(MouseButton button, IntVec2 const& position)
        : Event(GetStaticType())
        , m_button(button)
        , m_position(position) {}
    virtual ~EventMouseDown() {}

    static constexpr int GetStaticType() { return EVENTTYPE_MOUSE_DOWN; }

    MouseButton GetButton() const { return m_button; }
    IntVec2 const& GetPosition() const { return m_position; }

    std::unique_ptr<Event> Clone() const override {
        return std::make_unique<EventMouseDown>(m_button, m_position);
    }

public:
    MouseButton m_button;
    IntVec2 m_position;
};

class EventMouseUp : public Event {
public:
    EventMouseUp(MouseButton button, IntVec2 const& position)
        : Event(GetStaticType())
        , m_button(button)
        , m_position(position) {}
    virtual ~EventMouseUp() {}

    static constexpr int GetStaticType() { return EVENTTYPE_MOUSE_UP; }

    MouseButton GetButton() const { return m_button; }
    IntVec2 const& GetPosition() const { return m_position; }

    std::unique_ptr<Event> Clone() const override {
        return std::make_unique<EventMouseUp>(m_button, m_position);
    }

public:
    MouseButton m_button;
    IntVec2 m_position;
};

class EventMouseMove : public Event {
public:
    EventMouseMove(IntVec2 const& position, FloatVec2 const& delta)
        : Event(GetStaticType())
        , m_position(position)
        , m_delta(delta) {}
    virtual ~EventMouseMove() {}

    static constexpr int GetStaticType() { return EVENTTYPE_MOUSE_MOVE; }

    IntVec2 const& GetPosition() const { return m_position; }
    FloatVec2 const& GetDelta() const { return m_delta; }

    std::unique_ptr<Event> Clone() const override {
        return std::make_unique<EventMouseMove>(m_position, m_delta);
    }

public:
    IntVec2 m_position;
    FloatVec2 m_delta;
};

class EventMouseWheel : public Event {
public:
    EventMouseWheel(IntVec2 const& delta)
        : Event(GetStaticType())
        , m_delta(delta) {}
    virtual ~EventMouseWheel() {}

    static constexpr int GetStaticType() { return EVENTTYPE_MOUSE_WHEEL; }

    IntVec2 const& GetDelta() const { return m_delta; }

    std::unique_ptr<Event> Clone() const override {
        return std::make_unique<EventMouseWheel>(m_delta);
    }

public:
    IntVec2 m_delta;
};
