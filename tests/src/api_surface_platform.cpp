// Pins the method signatures of Application, Window, IPlatform, and Ticker.

#include "moth_graphics/moth_graphics.h"

#include <catch2/catch_all.hpp>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>

using namespace moth_graphics;
using namespace moth_graphics::platform;
using namespace moth_graphics::graphics;

TEST_CASE("Ticker method signatures are stable", "[api][platform][ticker]") {
    uint32_t (Ticker::*getFixed)() const = &Ticker::GetFixedTicks;
    void     (Ticker::*setRunning)(bool) = &Ticker::SetRunning;
    void     (Ticker::*tickSync)()       = &Ticker::TickSync;
    (void)getFixed; (void)setRunning; (void)tickSync;
    SUCCEED();
}

TEST_CASE("IPlatform method signatures are stable", "[api][platform][iplatform]") {
    bool (IPlatform::*startup)()                                          = &IPlatform::Startup;
    void (IPlatform::*shutdown)()                                         = &IPlatform::Shutdown;
    Context& (IPlatform::*getCtx)()                                       = &IPlatform::GetGraphicsContext;
    std::unique_ptr<Window> (IPlatform::*createWin)(
        std::string_view, int, int)                                       = &IPlatform::CreateWindow;
    std::unique_ptr<ImGuiContext> (IPlatform::*createImGui)()             = &IPlatform::CreateImGuiContext;
    (void)startup; (void)shutdown; (void)getCtx; (void)createWin; (void)createImGui;
    SUCCEED();
}

TEST_CASE("Application method signatures are stable", "[api][platform][application]") {
    void    (Application::*init)()              = &Application::Init;
    void    (Application::*run)()               = &Application::Run;
    Window* (Application::*getWin)()            = &Application::GetWindow;
    (void)init; (void)run; (void)getWin;
    SUCCEED();
}

TEST_CASE("Window inherits EventEmitter and IEventListener", "[api][platform][window]") {
    static_assert(std::is_base_of_v<EventEmitter, Window>);
    static_assert(std::is_base_of_v<moth_ui::IEventListener, Window>);
    SUCCEED();
}

TEST_CASE("Window method signatures are stable", "[api][platform][window]") {
    void (Window::*update)(uint32_t)                            = &Window::Update;
    void (Window::*draw)()                                      = &Window::Draw;
    graphics::SurfaceContext& (Window::*getSurface)() const     = &Window::GetSurfaceContext;
    void (Window::*setTitle)(std::string_view)                  = &Window::SetWindowTitle;
    bool (Window::*isMaximized)() const                         = &Window::IsMaximized;
    IntVec2 const& (Window::*getPos)() const                    = &Window::GetPosition;
    int  (Window::*getW)() const                                = &Window::GetWidth;
    int  (Window::*getH)() const                                = &Window::GetHeight;
    moth_ui::Context& (Window::*getMothCtx)() const             = &Window::GetMothContext;
    IGraphics& (Window::*getGraphics)() const                   = &Window::GetGraphics;
    TextureFactory& (Window::*getTexFactory)() const            = &Window::GetTextureFactory;
    bool (Window::*onEvent)(moth_ui::Event const&)              = &Window::OnEvent;
    void (Window::*pushLayer)(std::unique_ptr<moth_ui::Layer>)   = &Window::PushLayer;

    (void)update; (void)draw; (void)getSurface; (void)setTitle;
    (void)isMaximized; (void)getPos; (void)getW; (void)getH;
    (void)getMothCtx; (void)getGraphics; (void)getTexFactory;
    (void)onEvent; (void)pushLayer;
    SUCCEED();
}
