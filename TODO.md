# TODO

Issues identified during code review. Work through each by verifying against current code before fixing.

---

## Critical

### 1. `SetTarget()` dereferences `dynamic_cast` result without null check
**File:** `src/graphics/sdl/sdl_graphics.cpp`

In the `else` branch of `SetTarget()`, `dynamic_cast<Image*>(target)` is dereferenced immediately without checking whether the cast succeeded. If `target` is not an `Image` the dereference will crash. The `dynamic_pointer_cast<Texture>()` result further down does have an assert, but the first cast does not.

**Suggested fix:**
```cpp
auto* image = dynamic_cast<Image*>(target);
assert(image && "SetTarget: target is not an Image");
auto sdlImage = std::dynamic_pointer_cast<Texture>(image->GetTexture());
assert(sdlImage && "SetTarget: texture is not an SDL Texture");
auto sdlTexture = sdlImage->GetSDLTexture();
SDL_SetRenderTarget(m_surfaceContext.GetRenderer(), sdlTexture->GetImpl());
```

---

## Architectural / Known Issues (from NOTES.md)

- **Vulkan crashes on window close** unless all resources (images, fonts) linked to that context are destroyed first. Resources need to be per-window; assets created for one window will not work on another.
- **Vulkan exit cleanup crashes** — needs investigation.
- **SDL multi-window event handling** not yet properly implemented.
- **`SurfaceContext`** design could be improved — see NOTES.md for planned refactor.
