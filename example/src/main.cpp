#include "example_app.h"
#include "moth_graphics/moth_graphics.h"
#if !MOTH_GRAPHICS_DISABLE_VULKAN
#include "moth_graphics/platform/glfw/glfw_platform.h"
#endif
#if !MOTH_GRAPHICS_DISABLE_SDL
#include "moth_graphics/platform/sdl/sdl_platform.h"
#endif

#if MOTH_GRAPHICS_DISABLE_VULKAN && MOTH_GRAPHICS_DISABLE_SDL
#error "Both MOTH_GRAPHICS_DISABLE_VULKAN and MOTH_GRAPHICS_DISABLE_SDL are defined. At least one backend must be enabled."
#endif

int main(int argc, char* argv[]) {
#if !MOTH_GRAPHICS_DISABLE_VULKAN
    auto platform = std::make_unique<moth_graphics::platform::glfw::Platform>();
#else
    auto platform = std::make_unique<moth_graphics::platform::sdl::Platform>();
#endif
    platform->Startup();
    startExampleApp(*platform);
    platform->Shutdown();
    return 0;
}
