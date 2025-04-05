#include "events/event_window.h"
#include "platform/iplatform.h"
#include "platform/window.h"

void exampleMain(platform::IPlatform& platform) {
    auto window1 = platform.CreateWindow("Window 1", 640, 480);
    auto window2 = platform.CreateWindow("Window 2", 300, 200);

    auto& surfaceContext = window1->GetSurfaceContext();
    auto texture = surfaceContext.ImageFromFile("assets/images/playership.png");
    auto font = surfaceContext.FontFromFile("assets/fonts/pilotcommand.ttf", 38);

    bool closeWindow1 = false;
    bool closeWindow2 = false;

    window1->AddEventListener([&](moth_ui::Event const& event) {
        if (moth_ui::event_cast<EventRequestQuit>(event)) {
            closeWindow1 = true;
            return true;
        }
        return false;
    });

    window2->AddEventListener([&](moth_ui::Event const& event) {
        if (moth_ui::event_cast<EventRequestQuit>(event)) {
            closeWindow2 = true;
            return true;
        }
        return false;
    });

    while (window1 || window2) {
        if (window1) {
            auto& graphics = window1->GetGraphics();
            graphics.Begin();
            graphics.SetColor(graphics::BasicColors::Red);
            graphics.DrawFillRectF(MakeRect(0.0f, 0.0f, 100.0f, 100.0f));
            graphics.SetColor(graphics::BasicColors::White);
            auto destRect = MakeRect(0, 0, 640, 480);
            graphics.DrawImageTiled(*texture, destRect, nullptr, 1.0f);
            graphics.SetBlendMode(graphics::BlendMode::Alpha);
            graphics.SetColor(graphics::BasicColors::Green);
            graphics.DrawText("hello", *font, graphics::TextHorizAlignment::Center, graphics::TextVertAlignment::Middle, MakeRect(0, 0, 640, 480));
            window1->Update(30);
            window1->Draw();
            graphics.End();
        }
        if (window2) {
            auto& graphics = window2->GetGraphics();
            graphics.Begin();
            graphics.SetColor(graphics::BasicColors::Blue);
            graphics.Clear();
            window2->Update(30);
            window2->Draw();
            graphics.End();
        }
        if (closeWindow1) {
            texture = nullptr;
            font = nullptr;
            window1 = nullptr;
        }
        if (closeWindow2) {
            window2 = nullptr;
        }
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1ms);
    }
}
