#include "canyon.h"
#include "example/example_app.h"
#include "example/example_main.h"
#include "platform/glfw/glfw_platform.h"
#include "platform/sdl/sdl_platform.h"

int main(int argc, char* argv[]) {
    auto platform = std::make_unique<platform::sdl::Platform>();
    // auto platform = std::make_unique<platform::glfw::Platform>();
    platform->Startup();

    // startExampleApp(*platform);
    exampleMain(*platform);

    platform->Shutdown();
    return 0;
}
