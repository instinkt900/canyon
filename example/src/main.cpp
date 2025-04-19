#include "canyon/canyon.h"
#include "example_app.h"
#include "example_main.h"
#include "canyon/platform/glfw/glfw_platform.h"
#include "canyon/platform/sdl/sdl_platform.h"

int main(int argc, char* argv[]) {
    // auto platform = std::make_unique<canyon::platform::sdl::Platform>();
    auto platform = std::make_unique<canyon::platform::glfw::Platform>();
    platform->Startup();

    startExampleApp(*platform);
    // exampleMain(*platform);

    platform->Shutdown();
    return 0;
}
