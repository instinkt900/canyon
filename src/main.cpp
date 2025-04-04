#include "canyon.h"
#include "example/example.h"

int main(int argc, char* argv[]) {
    startExampleApp();

    // auto window = platform->CreateWindow("testing", 640, 480);
    // auto& surfaceContext = window->GetSurfaceContext();
    // auto texture = surfaceContext.ImageFromFile("assets/images/playership.png");
    // auto font = surfaceContext.FontFromFile("assets/fonts/pilotcommand.ttf", 38);
    //
    // bool running = true;
    // QuitListener ql(running);
    // window->AddEventListener(&ql);
    //
    // auto& graphics = window->GetGraphics();
    // while (running) {
    //     graphics.Begin();
    //     graphics.SetColor(graphics::BasicColors::Red);
    //     graphics.DrawFillRectF(MakeRect(0.0f, 0.0f, 100.0f, 100.0f));
    //     graphics.SetColor(graphics::BasicColors::White);
    //     auto destRect = MakeRect(0, 0, 640, 480);
    //     graphics.DrawImageTiled(*texture, destRect, nullptr, 1.0f);
    //     graphics.SetBlendMode(graphics::BlendMode::Alpha);
    //     graphics.SetColor(graphics::BasicColors::Green);
    //     graphics.DrawText("hello", *font, graphics::TextHorizAlignment::Center, graphics::TextVertAlignment::Middle, MakeRect(0, 0, 640, 480));
    //     window->Update(30);
    //     // window->Draw();
    //     graphics.End();
    //     using namespace std::chrono_literals;
    //     std::this_thread::sleep_for(1ms);
    // }

    return 0;
}
