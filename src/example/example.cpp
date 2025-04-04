#include "canyon.h"
#include "example/example_layer.h"
#include "platform/glfw/glfw_platform.h"
#include "platform/glfw/glfw_platform.h"
#include "platform/application.h"
#include "moth_ui/context.h"
#include <moth_ui/event_listener.h>

void startExampleApp() {
    // auto platform = std::make_unique<platform::sdl::Platform>();
    auto platform = std::make_unique<platform::glfw::Platform>();

    platform->Startup();
    
    auto application = std::make_unique<Application>(*platform);
    auto& window = application->GetWindow();
    auto& mothContext = window.GetMothContext();
    auto& graphics = window.GetGraphics();

    // load the fonts before layouts
    mothContext.GetFontFactory().LoadProject("assets/fonts.json");

    // add an example layer
    window.GetLayerStack().PushLayer(std::make_unique<TestLayer>(mothContext, graphics, "assets/layouts/basic.mothui"));

    // start the application
    application->TickSync();

    platform->Shutdown();
}
