#include "canyon/canyon.h"
#include "canyon/platform/application.h"
#include "canyon/platform/window.h"
#include "example_layer.h"
#include <moth_ui/events/event_listener.h>

void startExampleApp(canyon::platform::IPlatform& platform) {
    auto application = std::make_unique<canyon::platform::Application>(platform, "Example", 640, 480);
    application->Init();
    auto window = application->GetWindow();
    auto& mothContext = window->GetMothContext();
    auto& graphics = window->GetGraphics();
    //
    // // load the fonts before layouts
    mothContext.GetFontFactory().LoadProject("assets/fonts.json");
    //
    // // add an example layer
    window->GetLayerStack().PushLayer(std::make_unique<TestLayer>(mothContext, graphics, "assets/layouts/basic.mothui"));

    // start the application
    application->Run();
}
