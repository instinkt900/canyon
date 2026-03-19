# Canyon

[![Build Status](https://github.com/instinkt900/canyon/actions/workflows/build-test.yml/badge.svg)](https://github.com/instinkt900/canyon/actions/workflows/build-test.yml)
[![Upload Status](https://github.com/instinkt900/canyon/actions/workflows/upload-release.yml/badge.svg)](https://github.com/instinkt900/canyon/actions/workflows/upload-release.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

A C++ application and graphics framework built on top of [moth_ui](https://github.com/instinkt900/moth_ui). canyon provides a platform abstraction layer (windowing, event loop), two graphics backends (SDL2 and Vulkan), and the glue that connects moth_ui's UI system to a runnable application.

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
- [Related Projects](#related-projects)
- [License](#license)

---

## Overview

canyon provides:

- **Platform backends** — SDL2 and GLFW window/event loop implementations
- **Graphics backends** — SDL2 renderer and Vulkan (with SPIR-V shaders, render targets, font rendering via FreeType + HarfBuzz)
- **moth_ui integration** — bridges canyon's rendering to moth_ui's layout and animation system
- **Asset factories** — cached image and font loading with texture pack (atlas) support
- **ImGui integration** — docking and multi-viewport support via the Vulkan backend
- **spdlog logging** — structured logging throughout initialisation and window lifecycle

### AI Disclosure

AI agents (primarily Claude) are used as tools in this project for tasks such as refactoring, documentation writing, and test implementation. The architecture, design decisions, and direction of the project are human-driven. This is not a vibe-coded project.

---

## Architecture

### Platform layer

`IPlatform` initialises the windowing system and creates `Window` instances. Two implementations are provided:

- `canyon::platform::sdl::Platform` — SDL2 backend, uses the SDL renderer
- `canyon::platform::glfw::Platform` — GLFW backend, uses the Vulkan renderer

### Application

`Application` ties a platform, a window, and a fixed-timestep loop together. Subclass it and override the lifecycle hooks:

```cpp
#include <canyon/platform/application.h>
#if !CANYON_DISABLE_VULKAN
#include <canyon/platform/glfw/glfw_platform.h>
#else
#include <canyon/platform/sdl/sdl_platform.h>
#endif

class MyApp : public canyon::platform::Application {
public:
    MyApp(canyon::platform::IPlatform& platform)
        : Application(platform, "My Window", 1280, 720) {}

protected:
    void PostCreateWindow() override {
        // push your first layer onto GetWindow()->GetLayerStack()
    }
};

int main() {
#if !CANYON_DISABLE_VULKAN
    canyon::platform::glfw::Platform platform;
#else
    canyon::platform::sdl::Platform platform;
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
gfx.DrawImage(*image, destRect);
gfx.DrawText("Hello", *font, destRect);
gfx.End();
```

Render targets allow off-screen drawing:

```cpp
auto target = gfx.CreateTarget(256, 256);
gfx.SetTarget(target.get());
// ... draw into target ...
gfx.SetTarget(nullptr);
gfx.DrawImage(*target->GetImage(), destRect);
```

### Asset loading

Images and fonts are loaded through `AssetContext`, obtained from the window's surface context:

```cpp
auto& assets = window.GetSurfaceContext().GetAssetContext();
auto image = assets.ImageFromFile("assets/sprite.png");
auto font  = assets.FontFromFile("assets/fonts/roboto.ttf", 16);
```

For cached, atlas-aware loading use `ImageFactory` directly:

```cpp
canyon::graphics::ImageFactory imageFactory(window.GetSurfaceContext().GetAssetContext());
imageFactory.LoadTexturePack("assets/sprites.json");
auto sprite = imageFactory.GetImage("assets/sprites/player.png"); // sourced from atlas
```

### moth_ui integration

canyon bridges its rendering to moth_ui automatically. Push a `moth_ui::Layer` onto the window's layer stack and the animation and event systems work out of the box.

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

Add canyon as a dependency in your `conanfile.py`:

```python
def requirements(self):
    self.requires("canyon/<version>")
```

Then link against the `canyon` target in CMake:

```cmake
find_package(canyon REQUIRED)
target_link_libraries(my_app PRIVATE canyon::canyon)
```

Pass backend options at install time to control which backends are compiled into the canyon package your project links against:

```bash
conan install . -o canyon/*:disable_sdl=True
```

Or pin them permanently in your own `conanfile.py`:

```python
def configure(self):
    self.options["canyon"].disable_sdl = True
```

The `CANYON_DISABLE_SDL` / `CANYON_DISABLE_VULKAN` compile definitions are propagated **automatically** to any target that links against canyon (they are declared `PUBLIC`). Your own `#if` guards stay in sync with how canyon was built without any extra steps.

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

Conan profiles for both platforms are provided in `conan/profiles/`.

### Linux

SDL2, SDL_image, SDL_ttf, GLFW, FreeType, and HarfBuzz must come from the system package manager on Linux (mixing Conan-built and system copies of these libraries causes runtime conflicts via GTK3/GDK-Pixbuf):

```bash
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libglfw3-dev libfreetype-dev libharfbuzz-dev
```

`conan install` will also install these automatically when `tools.system.package_manager:mode=install` is set in the profile (already configured in `conan/profiles/linux_profile`).

```bash
conan install . --profile conan/profiles/linux_profile --build=missing -s build_type=Release
cmake --preset conan-release
cmake --build --preset conan-release
```

For a Debug build replace `Release` / `conan-release` with `Debug` / `conan-debug`.

### Windows

```bash
conan install . --profile conan/profiles/windows_profile --build=missing -s build_type=Release
cmake --preset conan-default
cmake --build --preset conan-release
```

### Disabling backends

Both backends are enabled by default. Pass `disable_vulkan=True` or `disable_sdl=True` as Conan options to strip a backend and its dependencies entirely. At least one backend must remain enabled.

**Vulkan only** (no SDL2 dependency):

```bash
conan install . --profile conan/profiles/linux_profile --build=missing -s build_type=Release -o canyon/*:disable_sdl=True
```

**SDL only** (no Vulkan/GLFW/FreeType/HarfBuzz dependency):

```bash
conan install . --profile conan/profiles/linux_profile --build=missing -s build_type=Release -o canyon/*:disable_vulkan=True
```

When a backend is disabled, the corresponding compile definition is propagated to all consumers:

| Option | Definition |
|---|---|
| `disable_vulkan=True` | `CANYON_DISABLE_VULKAN=1` |
| `disable_sdl=True` | `CANYON_DISABLE_SDL=1` |

Use these in your own code to guard backend-specific includes:

```cpp
#if !CANYON_DISABLE_VULKAN
#include <canyon/platform/glfw/glfw_platform.h>
#endif
#if !CANYON_DISABLE_SDL
#include <canyon/platform/sdl/sdl_platform.h>
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
conan create . --profile conan/profiles/linux_profile --build=missing -s build_type=Release
```

Consumers can then depend on `canyon/<version>` in their own `conanfile.py`.

---

## Related Projects

| Project | Description |
|---|---|
| [moth_ui](https://github.com/instinkt900/moth_ui) | Core UI library — node graph, keyframe animation, and event system that canyon builds on |
| [moth_editor](https://github.com/instinkt900/moth_editor) | Visual layout and animation editor — Flash-like authoring tool for creating `.mothui` layout files |

---

## License

MIT
