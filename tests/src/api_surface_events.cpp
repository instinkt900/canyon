// Pins the signatures of moth_graphics event classes and the EventEmitter
// interface.

#include "moth_graphics/moth_graphics.h"

#include <catch2/catch_all.hpp>
#include <functional>
#include <memory>

using namespace moth_graphics;

TEST_CASE("EventWindowSize method signatures are stable", "[api][events][window_size]") {
    int (EventWindowSize::*getW)() const = &EventWindowSize::GetWidth;
    int (EventWindowSize::*getH)() const = &EventWindowSize::GetHeight;
    (void)getW; (void)getH;
    SUCCEED();
}

TEST_CASE("EventWindowSize, EventRequestQuit, EventQuit, EventRenderDeviceReset, EventRenderTargetReset GetStaticType values are distinct",
          "[api][events][types]") {
    static_assert(EventWindowSize::GetStaticType()         == EVENTTYPE_WINDOWSIZE);
    static_assert(EventRequestQuit::GetStaticType()        == EVENTTYPE_REQUEST_QUIT);
    static_assert(EventQuit::GetStaticType()               == EVENTTYPE_QUIT);
    static_assert(EventRenderDeviceReset::GetStaticType()  == EVENTTYPE_RENDERDEVICERESET);
    static_assert(EventRenderTargetReset::GetStaticType()  == EVENTTYPE_RENDERTARGETRESET);
    SUCCEED();
}

TEST_CASE("LambdaHandle IsValid is stable", "[api][events][lambda_handle]") {
    bool (LambdaHandle::*isValid)() const = &LambdaHandle::IsValid;
    (void)isValid;

    LambdaHandle h;
    REQUIRE_FALSE(h.IsValid());
    SUCCEED();
}

TEST_CASE("EventEmitter method signatures are stable", "[api][events][emitter]") {
    void (EventEmitter::*addPtr)(moth_ui::IEventListener*)                           = &EventEmitter::AddEventListener;
    void (EventEmitter::*removePtr)(moth_ui::IEventListener*)                        = &EventEmitter::RemoveEventListener;
    LambdaHandle (EventEmitter::*addLambda)(
        std::function<bool(moth_ui::Event const&)> const&)                          = &EventEmitter::AddEventListener;
    void (EventEmitter::*removeLambda)(LambdaHandle const&)                         = &EventEmitter::RemoveEventListener;
    bool (EventEmitter::*emit)(moth_ui::Event const&)                               = &EventEmitter::EmitEvent;

    (void)addPtr; (void)removePtr; (void)addLambda; (void)removeLambda; (void)emit;
    SUCCEED();
}
