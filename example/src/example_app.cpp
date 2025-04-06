#include "canyon.h"
#include "example_layer.h"
#include "platform/application.h"
#include "platform/window.h"
#include <moth_ui/event_listener.h>

void startExampleApp(platform::IPlatform& platform) {
    auto application = std::make_unique<Application>(platform);
    auto& window = application->GetWindow();
    auto& mothContext = window.GetMothContext();
    auto& graphics = window.GetGraphics();

    // load the fonts before layouts
    mothContext.GetFontFactory().LoadProject("assets/fonts.json");

    // add an example layer
    window.GetLayerStack().PushLayer(std::make_unique<TestLayer>(mothContext, graphics, "assets/layouts/basic.mothui"));

    // start the application
    application->TickSync();
}
