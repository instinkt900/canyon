#include "canyon/canyon.h"
#include "example_app.h"
#include "example_main.h"
#if !CANYON_DISABLE_VULKAN
#include "canyon/platform/glfw/glfw_platform.h"
#endif
#if !CANYON_DISABLE_SDL
#include "canyon/platform/sdl/sdl_platform.h"
#endif

#if CANYON_DISABLE_VULKAN && CANYON_DISABLE_SDL
#error "Both CANYON_DISABLE_VULKAN and CANYON_DISABLE_SDL are defined. At least one backend must be enabled."
#endif

int main(int argc, char* argv[]) {
#if !CANYON_DISABLE_VULKAN
    auto platform = std::make_unique<canyon::platform::glfw::Platform>();
#else
    auto platform = std::make_unique<canyon::platform::sdl::Platform>();
#endif
    platform->Startup();

    startExampleApp(*platform);
    // exampleMain(*platform);

    platform->Shutdown();
    return 0;
}
