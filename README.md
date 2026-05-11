# Moth Graphics

[![Build Status](https://github.com/instinkt900/moth_graphics/actions/workflows/build-test.yml/badge.svg)](https://github.com/instinkt900/moth_graphics/actions/workflows/build-test.yml)
[![Upload Status](https://github.com/instinkt900/moth_graphics/actions/workflows/upload-release.yml/badge.svg)](https://github.com/instinkt900/moth_graphics/actions/workflows/upload-release.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

A C++17 application and graphics framework built on top of [moth_ui](https://github.com/instinkt900/moth_ui). moth_graphics provides a platform abstraction layer (windowing, event loop), two graphics backends (SDL2 and Vulkan), and the glue that connects moth_ui's UI system to a runnable application.

---

## Table of Contents

- [Overview](#overview)
  - [AI Disclosure](#ai-disclosure)
- [Architecture](#architecture)
  - [Platform layer](#platform-layer)
  - [Application](#application)
  - [Graphics](#graphics)
  - [Asset loading](#asset-loading)
  - [moth\_ui integration](#moth_ui-integration)
- [Dependencies](#dependencies)
- [Using with Conan](#using-with-conan)
- [Building](#building)
  - [Prerequisites](#prerequisites)
  - [Linux](#linux)
  - [Windows](#windows)
  - [Disabling backends](#disabling-backends)
- [Installing / Publishing](#installing--publishing)
- [Known Limitations](#known-limitations)
- [Related Projects](#related-projects)
- [License](#license)

---

## Overview

moth_graphics provides:

- **Platform backends** — SDL2 and GLFW window/event loop implementations
- **Graphics backends** — SDL2 renderer and Vulkan (with SPIR-V shaders, render targets, font rendering via FreeType + HarfBuzz)
- **moth_ui integration** — bridges moth_graphics's rendering to moth_ui's layout and animation system
- **Asset factories** — cached image and font loading with texture pack (atlas) support
- **ImGui integration** — docking on both backends via a platform-level `ImGuiContext`; multi-viewport support is opt-in (`Application::SetImGuiViewportsEnabled(true)` before `Init()`)
- **spdlog logging** — structured logging throughout initialisation and window lifecycle

### AI Disclosure

AI agents (primarily Claude) are used as tools in this project for tasks such as refactoring, documentation writing, and test implementation. The architecture, design decisions, and direction of the project are human-driven. This is not a vibe-coded project.

---

## Architecture

### Platform layer

`IPlatform` initializes the windowing system and creates `Window` instances. Two implementations are provided:

- `moth_graphics::platform::sdl::Platform` — SDL2 backend, uses the SDL renderer
- `moth_graphics::platform::glfw::Platform` — GLFW backend, uses the Vulkan renderer

### Application

`Application` ties a platform, a window, and a fixed-timestep loop together. Subclass it and override the lifecycle hooks:

```cpp
#include <moth_graphics/platform/application.h>
#if !MOTH_GRAPHICS_DISABLE_VULKAN
#include <moth_graphics/platform/glfw/glfw_platform.h>
#else
#include <moth_graphics/platform/sdl/sdl_platform.h>
#endif

class MyApp : public moth_graphics::platform::Application {
public:
    MyApp(moth_graphics::platform::IPlatform& platform)
        : Application(platform, "My Window", 1280, 720) {}

protected:
    void PostCreateWindow() override {
        // push your first layer onto GetWindow()->GetLayerStack()
    }
};

int main() {
#if !MOTH_GRAPHICS_DISABLE_VULKAN
    moth_graphics::platform::glfw::Platform platform;
#else
    moth_graphics::platform::sdl::Platform platform;
#endif
    platform.Startup();
    MyApp app(platform);
    app.Init();
    app.Run();
    platform.Shutdown();
}
```

### Graphics

`IGraphics` is the 2D drawing interface. All draw calls go between `Begin()` and `End()`:

```cpp
auto& gfx = window.GetGraphics();
gfx.Begin();
gfx.SetColor({ 1, 0, 0, 1 });
gfx.DrawFillRectF({ { 10, 10 }, { 110, 110 } });
gfx.DrawImage(image, destRect);
gfx.DrawText("Hello", *font, destRect);
gfx.End();
```

Render targets allow off-screen drawing:

```cpp
auto target = gfx.CreateTarget(256, 256);
gfx.SetTarget(target.get());
// ... draw into target ...
gfx.SetTarget(nullptr);
gfx.DrawImage(target->GetImage(), destRect);
```

### Asset loading

Textures and fonts are loaded through `AssetContext`, obtained from the window's surface context. `Image` is a cheap value type — wrap a loaded texture in one to draw it:

```cpp
auto& assets = window.GetSurfaceContext().GetAssetContext();
auto texture = assets.TextureFromFile("assets/sprite.png");
moth_graphics::graphics::Image image{ std::move(texture) };
auto font = assets.FontFromFile("assets/fonts/roboto.ttf", 16);
```

For cached, atlas-aware loading use the `TextureFactory` exposed by the asset context:

```cpp
auto& textureFactory = window.GetSurfaceContext().GetAssetContext().GetTextureFactory();
textureFactory.LoadTexturePack("assets/sprites.json");
auto texture = textureFactory.GetTexture("assets/sprites/player.png");
moth_graphics::graphics::Image sprite{ texture, textureFactory.GetTextureRect("assets/sprites/player.png") };
```

### moth_ui integration

moth_graphics bridges its rendering to moth_ui automatically. Push a `moth_ui::Layer` onto the window's layer stack and the animation and event systems work out of the box.

---

## Dependencies

| Dependency | Source | Notes |
|---|---|---|
| moth_ui ≥ 1.0.0 | Conan | Core UI library |
| Vulkan headers 1.3.243 | Conan | |
| Vulkan loader 1.3.243 | Conan | |
| Vulkan Memory Allocator 3.0.1 | Conan | |
| spdlog ≥ 1.14 | Conan | |
| SDL2, SDL2_image, SDL2_ttf | System (Linux) / Conan (Windows) | |
| GLFW | System (Linux) / Conan (Windows) | |
| FreeType | System (Linux) / Conan (Windows) | |
| HarfBuzz | System (Linux) / Conan (Windows) | |
| Dear ImGui | Bundled (`external/imgui/`) | |

---

## Using with Conan

Add moth_graphics as a dependency in your `conanfile.py`:

```python
def requirements(self):
    self.requires("moth_graphics/<version>")
```

Then link against the `moth_graphics` target in CMake:

```cmake
find_package(moth_graphics REQUIRED)
target_link_libraries(my_app PRIVATE moth_graphics::moth_graphics)
```

Pass backend options at install time to control which backends are compiled into the moth_graphics package your project links against:

```bash
conan install . -o moth_graphics/*:disable_sdl=True
```

Or pin them permanently in your own `conanfile.py`:

```python
def configure(self):
    self.options["moth_graphics"].disable_sdl = True
```

The `MOTH_GRAPHICS_DISABLE_SDL` / `MOTH_GRAPHICS_DISABLE_VULKAN` compile definitions are propagated **automatically** to any target that links against moth_graphics (they are declared `PUBLIC`). Your own `#if` guards stay in sync with how moth_graphics was built without any extra steps.

---

## Building

### Prerequisites

Set up a Python virtual environment and install Conan:

```bash
# Linux / macOS
python3 -m venv .venv
source .venv/bin/activate
pip install conan

# Windows (PowerShell)
python3 -m venv .venv
.\.venv\Scripts\Activate.ps1
pip install conan
```

**C++17 is required.** A `.conan/profile` is provided that sets `compiler.cppstd=17` and configures Conan to install system packages automatically (`tools.system.package_manager:mode=install`). This profile is used in CI and can be used directly or as a reference when building locally.

### Linux

SDL2, SDL_image, SDL_ttf, GLFW, FreeType, and HarfBuzz must come from the system package manager on Linux (mixing Conan-built and system copies of these libraries causes runtime conflicts via GTK3/GDK-Pixbuf).

Using `.conan/profile`, Conan will install these automatically via `apt`:

```bash
conan install . -pr .conan/profile -s build_type=Release --build=missing
cmake --preset conan-release
cmake --build --preset conan-release
```

If you'd rather install them yourself first:

```bash
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libglfw3-dev libfreetype-dev libharfbuzz-dev
```

For a Debug build replace `Release` / `conan-release` with `Debug` / `conan-debug`.

### Windows

```bash
conan install . -pr .conan/profile -s build_type=Release --build=missing
cmake --preset conan-default
cmake --build --preset conan-release
```

### Disabling backends

Both backends are enabled by default. Pass `disable_vulkan=True` or `disable_sdl=True` as Conan options to strip a backend and its dependencies entirely. At least one backend must remain enabled.

**Vulkan only** (no SDL2 dependency):

```bash
conan install . -pr .conan/profile -s build_type=Release --build=missing -o moth_graphics/*:disable_sdl=True
```

**SDL only** (no Vulkan/GLFW/FreeType/HarfBuzz dependency):

```bash
conan install . -pr .conan/profile -s build_type=Release --build=missing -o moth_graphics/*:disable_vulkan=True
```

When a backend is disabled, the corresponding compile definition is propagated to all consumers:

| Option | Definition |
|---|---|
| `disable_vulkan=True` | `MOTH_GRAPHICS_DISABLE_VULKAN=1` |
| `disable_sdl=True` | `MOTH_GRAPHICS_DISABLE_SDL=1` |

Use these in your own code to guard backend-specific includes:

```cpp
#if !MOTH_GRAPHICS_DISABLE_VULKAN
#include <moth_graphics/platform/glfw/glfw_platform.h>
#endif
#if !MOTH_GRAPHICS_DISABLE_SDL
#include <moth_graphics/platform/sdl/sdl_platform.h>
#endif
```

---

## Installing / Publishing

To install the library locally for use by another project:

```bash
cmake --install build --config Release --prefix=<install_path>
```

To publish via Conan:

```bash
conan create . -pr .conan/profile -s build_type=Release --build=missing
```

Consumers can then depend on `moth_graphics/<version>` in their own `conanfile.py`.

---

## Known Limitations

- **Single primary window.** `Application` manages one window. Multiple windows can be created manually via `IPlatform::CreateWindow`, but they are not tracked by the application lifecycle and must be managed by the caller.

- **Vulkan pixel format may be incorrect for sRGB content.** The pipeline uses `VK_FORMAT_B8G8R8A8_UNORM` (BGRA byte order, linear encoding). If sRGB-correct blending is needed, this should be changed to `VK_FORMAT_B8G8R8A8_SRGB` (same byte order, sRGB encoding). The `VK_FORMAT_R8G8B8A8_SRGB` variant was previously considered but would also require a byte-order swap. See `vulkan_graphics_pipeline.cpp`. This is unlikely to affect typical UI rendering.

- **UI node trees are single-threaded.** This is a constraint inherited from moth_ui. Update, draw, and event dispatch on a `LayerStack` must all happen on the same thread. See the moth_ui documentation for the threading model of its other components.

- **Linux system library constraint.** SDL2, SDL_image, SDL_ttf, GLFW, FreeType, and HarfBuzz must come from the system package manager on Linux (not Conan). See [Building → Linux](#linux) for details.

---

## Related Projects

| Project | Description |
|---|---|
| [moth_ui](https://github.com/instinkt900/moth_ui) | Core UI library — node graph, keyframe animation, and event system |
| moth_graphics | *(this project)* Graphics and application framework built on moth_ui — SDL2 and Vulkan backends, window management, and a layer stack |
| [moth_editor](https://github.com/instinkt900/moth_editor) | Visual layout and animation editor — Flash-like authoring tool for creating moth_ui layout files |
| [moth_packer](https://github.com/instinkt900/moth_packer) | Command-line texture atlas packer for images and moth_ui layouts |

---

## License

MIT
