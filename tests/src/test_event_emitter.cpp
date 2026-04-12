#include "moth_graphics/events/event_emitter.h"
#include "moth_graphics/events/event_window.h"

#include <moth_ui/events/event.h>
#include <moth_ui/events/event_listener.h>

#include <catch2/catch_all.hpp>
#include <memory>

using namespace moth_graphics;
using namespace moth_ui;

// ---------------------------------------------------------------------------
// Concrete listener for testing
// ---------------------------------------------------------------------------

namespace {
    class CountingListener : public EventListener {
    public:
        int count = 0;

        bool OnEvent(Event const& /*event*/) override {
            ++count;
            return false;  // don't consume, let all listeners fire
        }
    };

    class ConsumingListener : public EventListener {
    public:
        bool OnEvent(Event const& /*event*/) override {
            return true;
        }
    };
}

// ---------------------------------------------------------------------------
// Pointer-based registration
// ---------------------------------------------------------------------------

TEST_CASE("AddEventListener(ptr) and EmitEvent delivers to listener", "[emitter][ptr]") {
    EventEmitter emitter;
    CountingListener listener;
    emitter.AddEventListener(&listener);

    EventRequestQuit ev;
    emitter.EmitEvent(ev);
    REQUIRE(listener.count == 1);
}

TEST_CASE("RemoveEventListener(ptr) stops delivery", "[emitter][ptr]") {
    EventEmitter emitter;
    CountingListener listener;
    emitter.AddEventListener(&listener);
    emitter.RemoveEventListener(&listener);

    EventRequestQuit ev;
    emitter.EmitEvent(ev);
    REQUIRE(listener.count == 0);
}

TEST_CASE("Multiple pointer listeners all receive the event", "[emitter][ptr]") {
    EventEmitter emitter;
    CountingListener a, b, c;
    emitter.AddEventListener(&a);
    emitter.AddEventListener(&b);
    emitter.AddEventListener(&c);

    EventRequestQuit ev;
    emitter.EmitEvent(ev);
    REQUIRE(a.count == 1);
    REQUIRE(b.count == 1);
    REQUIRE(c.count == 1);
}

// ---------------------------------------------------------------------------
// Lambda-based registration
// ---------------------------------------------------------------------------

TEST_CASE("AddEventListener(lambda) delivers event to lambda", "[emitter][lambda]") {
    EventEmitter emitter;
    int callCount = 0;
    emitter.AddEventListener([&](Event const&) {
        ++callCount;
        return false;
    });

    EventQuit ev;
    emitter.EmitEvent(ev);
    REQUIRE(callCount == 1);
}

TEST_CASE("LambdaHandle::IsValid is true for registered lambda, false for default", "[emitter][lambda_handle]") {
    EventEmitter emitter;
    LambdaHandle def;
    REQUIRE_FALSE(def.IsValid());

    LambdaHandle h = emitter.AddEventListener([](Event const&) { return false; });
    REQUIRE(h.IsValid());
}

TEST_CASE("RemoveEventListener(handle) stops delivery", "[emitter][lambda]") {
    EventEmitter emitter;
    int callCount = 0;
    LambdaHandle h = emitter.AddEventListener([&](Event const&) {
        ++callCount;
        return false;
    });
    emitter.RemoveEventListener(h);

    EventQuit ev;
    emitter.EmitEvent(ev);
    REQUIRE(callCount == 0);
}

TEST_CASE("RemoveEventListener(invalid handle) is a no-op", "[emitter][lambda]") {
    EventEmitter emitter;
    int callCount = 0;
    emitter.AddEventListener([&](Event const&) {
        ++callCount;
        return false;
    });
    LambdaHandle invalid;
    emitter.RemoveEventListener(invalid);  // must not crash

    EventQuit ev;
    emitter.EmitEvent(ev);
    REQUIRE(callCount == 1);
}

// ---------------------------------------------------------------------------
// EmitEvent return value
// ---------------------------------------------------------------------------

TEST_CASE("EmitEvent returns false when no listener consumes", "[emitter][handled]") {
    EventEmitter emitter;
    CountingListener listener;
    emitter.AddEventListener(&listener);

    EventQuit ev;
    REQUIRE_FALSE(emitter.EmitEvent(ev));
}

TEST_CASE("EmitEvent returns true when a listener consumes the event", "[emitter][handled]") {
    EventEmitter emitter;
    ConsumingListener listener;
    emitter.AddEventListener(&listener);

    EventQuit ev;
    REQUIRE(emitter.EmitEvent(ev));
}

TEST_CASE("EmitEvent returns false with no listeners registered", "[emitter][handled]") {
    EventEmitter emitter;
    EventQuit ev;
    REQUIRE_FALSE(emitter.EmitEvent(ev));
}
