#include "example_app.h"
#include "example_layer.h"
#include "canyon/canyon.h"
#include "canyon/graphics/surface_context.h"
#include "canyon/platform/application.h"
#include "canyon/platform/window.h"

void startExampleApp(canyon::platform::IPlatform& platform) {
    auto application = std::make_unique<canyon::platform::Application>(platform, "Canyon Example", 640, 480);
    application->Init();

    auto* window = application->GetWindow();
    auto& graphics = window->GetGraphics();
    auto& assets = window->GetSurfaceContext().GetAssetContext();

    window->GetLayerStack().PushLayer(std::make_unique<ExampleLayer>(graphics, assets));

    application->Run();
}
