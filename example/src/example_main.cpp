#include <memory>
#include "canyon/events/event_window.h"
#include "canyon/platform/iplatform.h"
#include "canyon/platform/window.h"

#include <thread>

void exampleMain(canyon::platform::IPlatform& platform) {
    auto window1 = platform.CreateWindow("Window 1", 300, 200);
    auto window2 = platform.CreateWindow("Window 2", 300, 200);

    auto& surfaceContext1 = window1->GetSurfaceContext();
    auto& surfaceContext2 = window2->GetSurfaceContext();
    auto texture = surfaceContext1.ImageFromFile("assets/images/playership.png");
    auto font1 = surfaceContext1.FontFromFile("assets/fonts/pilotcommand.ttf", 38);
    auto font2 = surfaceContext2.FontFromFile("assets/fonts/pilotcommand.ttf", 38);

    bool closeWindow1 = false;
    bool closeWindow2 = false;

    window1->AddEventListener([&](moth_ui::Event const& event) {
        if (moth_ui::event_cast<canyon::EventRequestQuit>(event)) {
            closeWindow1 = true;
            return true;
        }
        return false;
    });

    window2->AddEventListener([&](moth_ui::Event const& event) {
        if (moth_ui::event_cast<canyon::EventRequestQuit>(event)) {
            closeWindow2 = true;
            return true;
        }
        return false;
    });

    while (window1 || window2) {
        if (window1) {
            auto& graphics = window1->GetGraphics();
            graphics.Begin();
            graphics.SetColor(canyon::graphics::BasicColors::White);
            graphics.Clear();
            auto const destRect = canyon::MakeRect(0, 0, window1->GetWidth(), window1->GetHeight());
            graphics.DrawImageTiled(*texture, destRect, nullptr, 1.0f);
            graphics.SetBlendMode(canyon::graphics::BlendMode::Alpha);
            graphics.SetColor(canyon::graphics::BasicColors::Blue);
            graphics.DrawText("hello", *font1, canyon::graphics::TextHorizAlignment::Center, canyon::graphics::TextVertAlignment::Middle, canyon::MakeRect(0, 0, window1->GetWidth(), window1->GetHeight()));
            window1->Update(30);
            window1->Draw();
            graphics.End();
        }
        if (window2) {
            auto& graphics = window2->GetGraphics();
            graphics.Begin();
            graphics.SetColor(canyon::graphics::BasicColors::Blue);
            graphics.Clear();
            graphics.SetColor(canyon::graphics::BasicColors::White);
            graphics.DrawText("world", *font2, canyon::graphics::TextHorizAlignment::Center, canyon::graphics::TextVertAlignment::Middle, canyon::MakeRect(0, 0, window2->GetWidth(), window2->GetHeight()));
            window2->Update(30);
            window2->Draw();
            graphics.End();
        }
        if (closeWindow1) {
            texture = nullptr;
            font1 = nullptr;
            window1 = nullptr;
        }
        if (closeWindow2) {
            font2 = nullptr;
            window2 = nullptr;
        }
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1ms);
    }
}
