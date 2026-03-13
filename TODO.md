# TODO

Issues identified during code review. Work through each by verifying against current code before fixing.

---

## Critical

### 1. Unchecked `glfwCreateWindow()` return value
**File:** `src/platform/glfw/glfw_window.cpp`

`glfwCreateWindow()` can return `nullptr` on failure. The result is immediately passed to `glfwSetWindowUserPointer()` and used to register callbacks without any null check. All subsequent GLFW calls in the constructor would receive a null pointer and crash.

---

### 2. GLFW callbacks cast user pointer without null check
**File:** `src/platform/glfw/glfw_window.cpp`

All static callback lambdas (window pos, size, maximise, key, cursor pos, mouse button) do:
```cpp
Window* app = static_cast<Window*>(glfwGetWindowUserPointer(window));
app->m_someField = ...;  // direct dereference, no null check
```
If the user pointer is somehow null the dereference will crash.

---

### 3. `SetTarget()` dereferences `dynamic_cast` result without null check
**File:** `src/graphics/sdl/sdl_graphics.cpp`

In the `else` branch of `SetTarget()`:
```cpp
auto sdlImage = std::dynamic_pointer_cast<Texture>(dynamic_cast<Image*>(target)->GetTexture());
auto sdlTexture = sdlImage->GetSDLTexture();
```
- `dynamic_cast<Image*>(target)` could return `nullptr` if `target` is not an `Image` — dereferenced immediately
- `dynamic_pointer_cast<Texture>()` result (`sdlImage`) is not checked before `->GetSDLTexture()`

---

### 4. Vulkan graphics context stack initialised with `nullptr`
**File:** `src/graphics/vulkan/vulkan_graphics.cpp`

The constructor pushes `nullptr` onto `m_contextStack`. Every rendering method then calls `m_contextStack.top()` and dereferences the result directly. If `Begin()` is not called first the dereference of `nullptr` will crash. No runtime guard exists.

---

## Medium

### 5. `glfwCreateWindowSurface()` return value not checked
**File:** `src/platform/glfw/glfw_window.cpp`

The `VkResult` returned by `glfwCreateWindowSurface()` is discarded. A failure here would silently produce an invalid `VkSurfaceKHR` and likely crash later during swapchain creation.

---

### 6. SDL window leaked if renderer creation fails
**File:** `src/platform/sdl/sdl_window.cpp`

In `CreateWindow()`:
```cpp
if (nullptr == (m_window = SDL_CreateWindow(...))) { return false; }
if (nullptr == (m_renderer = SDL_CreateRenderer(m_window, ...))) {
    return false;  // m_window allocated but never destroyed
}
```
If renderer creation fails, `m_window` is leaked.

---

### 7. `Application` copy/move operators not explicitly deleted
**File:** `include/canyon/platform/application.h`

`Application` holds `unique_ptr` members making it non-copyable by default, but copy and move operators are not explicitly `= delete`. This should be made explicit for clarity and to prevent accidental misuse.

---

## Low

### 8. `m_ftLibrary` not initialised to `nullptr` in header
**File:** `include/canyon/graphics/vulkan/vulkan_context.h`

`FT_Library m_ftLibrary;` is declared without an initialiser. Peer members (`m_vkInstance`, `m_vkDebugMessenger`) are initialised to `VK_NULL_HANDLE`. `m_ftLibrary` should be initialised to `nullptr` for consistency and safety.

---

### 9. CI workflow hardcodes internal artifactory URL
**File:** `.github/workflows/build-test.yml`

The Conan remote URL points to an internal artifactory instance. Credentials are handled via secrets (correct), but the URL itself reveals internal infrastructure if the repository is ever made public. Consider moving the URL to a secret or repository variable as well.

---

## Architectural / Known Issues (from NOTES.md)

- **Vulkan crashes on window close** unless all resources (images, fonts) linked to that context are destroyed first. Resources need to be per-window; assets created for one window will not work on another.
- **Vulkan exit cleanup crashes** — needs investigation.
- **SDL multi-window event handling** not yet properly implemented.
- **`SurfaceContext`** design could be improved — see NOTES.md for planned refactor.
