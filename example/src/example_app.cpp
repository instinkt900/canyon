#include "example_app.h"
#include "example_layer.h"
#include "moth_graphics/moth_graphics.h"
#include "moth_graphics/graphics/surface_context.h"
#include "moth_graphics/platform/application.h"
#include "moth_graphics/platform/window.h"

void startExampleApp(moth_graphics::platform::IPlatform& platform) {
    auto application = std::make_unique<moth_graphics::platform::Application>(platform, "Moth Graphics Example", 640, 480);
    application->Init();

    auto* window = application->GetWindow();
    auto& graphics = window->GetGraphics();
    auto& assets = window->GetSurfaceContext().GetAssetContext();

    window->GetLayerStack().PushLayer(std::make_unique<ExampleLayer>(graphics, assets));

    application->Run();
}
