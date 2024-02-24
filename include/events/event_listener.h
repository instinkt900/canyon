#pragma once

class Event;

class EventListener {
public:
    virtual bool OnEvent(Event const& event) = 0;
};
