#pragma once

#include "events/event.h"

enum WindowEventType : int {
    EVENTTYPE_RENDERDEVICERESET = EventType::EVENTTYPE_USER,
    EVENTTYPE_RENDERTARGETRESET,
    EVENTTYPE_WINDOWSIZE,
    EVENTTYPE_REQUEST_QUIT,
    EVENTTYPE_QUIT,
};

class EventRenderDeviceReset : public Event {
public:
    EventRenderDeviceReset()
        : Event(GetStaticType()) {}
    virtual ~EventRenderDeviceReset() = default;

    static constexpr int GetStaticType() { return EVENTTYPE_RENDERDEVICERESET; }

    std::unique_ptr<Event> Clone() const override {
        return std::make_unique<EventRenderDeviceReset>();
    }
};

class EventRenderTargetReset : public Event {
public:
    EventRenderTargetReset()
        : Event(GetStaticType()) {}
    virtual ~EventRenderTargetReset() = default;

    static constexpr int GetStaticType() { return EVENTTYPE_RENDERTARGETRESET; }

    std::unique_ptr<Event> Clone() const override {
        return std::make_unique<EventRenderTargetReset>();
    }
};

class EventWindowSize : public Event {
public:
    EventWindowSize(int width, int height)
        : Event(GetStaticType())
        , m_width(width)
        , m_height(height) {}
    virtual ~EventWindowSize() = default;

    static constexpr int GetStaticType() { return EVENTTYPE_WINDOWSIZE; }

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    std::unique_ptr<Event> Clone() const override {
        return std::make_unique<EventWindowSize>(m_width, m_height);
    }

private:
    int m_width = 0;
    int m_height = 0;
};

class EventRequestQuit : public Event {
public:
    EventRequestQuit()
        : Event(GetStaticType()) {}
    virtual ~EventRequestQuit() = default;

    static constexpr int GetStaticType() { return EVENTTYPE_REQUEST_QUIT; }

    std::unique_ptr<Event> Clone() const override {
        return std::make_unique<EventRequestQuit>();
    }
};

class EventQuit : public Event {
public:
    EventQuit()
        : Event(GetStaticType()) {}
    virtual ~EventQuit() = default;

    static constexpr int GetStaticType() { return EVENTTYPE_QUIT; }

    std::unique_ptr<Event> Clone() const override {
        return std::make_unique<EventQuit>();
    }
};
