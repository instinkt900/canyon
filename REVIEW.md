# moth_graphics Code Review — 2026-04-25

Brutal, thorough tech-lead review of the moth_graphics library (v0.10.1). All findings are based on reading the full source tree: headers, implementations, tests, and build configuration.

---

## Critical: Bugs and Crash Risks

### 1. Vulkan DrawText corrupts blend mode state

**File:** `src/graphics/vulkan/vulkan_graphics.cpp:362`

```cpp
void Graphics::DrawText(...) {
    auto* context = CurrentContext();
    context->m_currentBlendMode = BlendMode::Alpha; // force alpha blending for text
    ...
}
```

This mutates `m_currentBlendMode` on the context and never restores it. Every draw call after `DrawText` in the same frame will use alpha blending regardless of what was set. The SDL backend does NOT have this bug — it leaves blend state alone and relies on the caller having set it.

**Fix:** Save and restore the previous blend mode, or set it per-glyph-batch and restore after the draw.

### 2. dynamic_cast<T&> throws std::bad_cast on type mismatch

**Files:** `src/graphics/sdl/sdl_graphics.cpp:111`, `src/graphics/vulkan/vulkan_graphics.cpp:167`

Both `DrawImage` implementations use:
```cpp
auto& sdlImage = dynamic_cast<Image&>(image);   // throws if wrong type
auto& vulkanImage = dynamic_cast<Image&>(image); // same
```

If a Vulkan image is passed to the SDL backend (or vice versa), this throws `std::bad_cast` — there is no catch block. This can happen trivially if someone caches an `IImage*` and the window is recreated with a different backend.

**Fix:** Use the pointer form and return early on null, the same way `MothRenderer::RenderImage` already does:
```cpp
auto* impl = dynamic_cast<Image*>(&image);
if (!impl) return;
```

Same issue exists in `DrawToPNG` (both backends), `DrawText` (both backends), and `SetTarget` (which uses `dynamic_cast` + `assert`).

### 3. Vulkan Begin() context stack is not guarded against double-Begin

**File:** `src/graphics/vulkan/vulkan_graphics_frame.cpp:12`

`BeginContext` pushes onto `m_contextStack` unconditionally. If `Begin()` is called twice without an intervening `End()`, the stack grows: `nullptr → defaultContext → defaultContext`. The first `End()` only pops one, leaving the stack in a corrupted state where `defaultContext` remains. The next `Begin()` pushes a third, and so on.

**Fix:** Assert in `BeginContext` that `m_contextStack.size() == 1` (the sentinel nullptr) before pushing.

---

## High Severity: Design Problems

### 4. SpriteSheetFactory::GetSpriteSheet is a 180-line monolith

**File:** `src/graphics/spritesheet_factory.cpp`

The entire JSON parsing, validation, image loading, and clip construction is one function. It's wrapped in `//NOLINTBEGIN(readability-function-cognitive-complexity)`. This makes the code hard to test (you can't test frame parsing independently of image loading), hard to read, and fragile to modify.

**Fix:** Extract `ParseFrames`, `ParseClips`, `ParseClipEntry`, `ParseLoopType` into private static/helper functions. Same applies to `ImageFactory::LoadTexturePack` (~100 lines, same NOLINT suppression).

### 5. Commented-out dead code

**Files:** `include/moth_graphics/graphics/sdl/sdl_graphics.h:36-37`, `include/moth_graphics/graphics/vulkan/vulkan_graphics.h:67-68`, `src/graphics/sdl/sdl_graphics.cpp:60-72`

Both graphics backends have commented-out method stubs for per-image blend mode and color modulation:
```cpp
// void Graphics::SetBlendMode(std::shared_ptr<graphics::IImage> target, graphics::BlendMode mode) {
//     ...
// }
```

These have been dead long enough to be wrapped in `//` blocks across multiple files. Either finish the feature or delete the corpses.

### 6. Empty Context base class

**File:** `include/moth_graphics/graphics/context.h`

```cpp
class Context {
public:
    virtual ~Context() = default;
};
```

This class has no methods, no data, no purpose beyond being a polymorphic base. It exists solely so `SDLPlatform::GetGraphicsContext()` can return a `Context&` for lazy-init. The Vulkan `Context` class does have real members (instance, device, queue, etc.), but `SDL::Context` is also nearly empty.

**Fix:** Either give `Context` a real interface, or eliminate it and have `IPlatform::GetGraphicsContext()` return something meaningful, or use `std::any`/type-erased init token.

### 7. Sprite::m_currentFrame has dual meaning

**File:** `include/moth_graphics/graphics/sprite.h:89`

```cpp
int m_currentFrame = 0;  // Clip-sequence position when clip is active; raw atlas index otherwise.
```

The comment itself is a code smell. When a clip is active, `m_currentFrame` indexes into `ClipDesc::frames`. When no clip is active, it indexes directly into the atlas. `GetCurrentFrame()` has the dual-path logic. This is confusing and error-prone — a future maintainer will get this wrong.

**Fix:** Separate the two states with `std::variant<int, int>` (clip step index | atlas frame index), or use two separate members and a bool/enum discriminant.

### 8. MothRenderer sets texture filter redundantly on every image draw

**File:** `src/graphics/moth_ui/moth_renderer.cpp:126-129`

```cpp
if (auto texture = internalImage.GetTexture()) {
    auto const gfxFilter = ToGraphicsFilter(m_textureFilter.top());
    texture->SetFilter(gfxFilter, gfxFilter);
}
```

This is called on every `RenderImage`, even for the same texture, same filter, consecutive frames. The SDL `SetFilter` implementation already guards against redundant mode changes (compares `m_scaleMode` before flushing), so the actual GPU cost is minimal — but the per-draw `dynamic_cast` + virtual call overhead adds up.

### 9. Vulkan Clear() draws a fullscreen rectangle

**File:** `src/graphics/vulkan/vulkan_graphics.cpp:135-137`

```cpp
void Graphics::Clear() {
    auto* context = CurrentContext();
    DrawFillRectF(...huge rect...);
}
```

This issues 6 vertices through the full pipeline instead of using `VK_ATTACHMENT_LOAD_OP_CLEAR`. The render pass is intentionally set to `LOAD_OP_LOAD` (with a comment "TODO this might have to change?" on the format line), so every frame loads the previous frame's contents from memory unnecessarily.

**Fix:** Use `VK_ATTACHMENT_LOAD_OP_CLEAR` on the swapchain render pass, or at minimum for the first draw of the frame.

---

## Medium Severity: Code Quality and Maintainability

### 10. DummyImage duplicated across test files

**Files:** `tests/src/test_sprite.cpp:20-26`, `tests/src/test_spritesheet.cpp:15-22`

The same 15-line `DummyImage` stub is copy-pasted. If `IImage` gains a new pure virtual method, both copies need updating.

**Fix:** Move to a shared test header (`tests/src/test_utils.h`).

### 11. Cache keys use lexical normalisation, not canonical paths

**Files:** `src/graphics/spritesheet_factory.cpp:23`, `src/graphics/image_factory.cpp:108`

Both use `lexically_normal()` instead of `weakly_canonical()` or `canonical()`. Symlinks to the same file produce different cache keys → duplicate GPU resources.

**Fix:** Use `std::filesystem::weakly_canonical()` (C++17) and handle the error case.

### 12. Hardcoded vertex/glyph capacity limits

**File:** `src/graphics/vulkan/vulkan_graphics_frame.cpp:8-10`

```cpp
static constexpr uint32_t kVertexBufferCapacity = 1024;
static constexpr uint32_t kMaxGlyphCount = 1024;
```

1024 vertices is not much — a few rotated text strings with border/background rects will exhaust this. Glyph limit is silently hit with a `spdlog::warn` and truncation. Neither is configurable.

### 13. Inconsistent error handling strategy

Some code throws exceptions (SDL Window constructor, Vulkan `ToVulkan` default case), some returns `nullptr` (all factories, `Sprite::Create`), some logs + continues (clip parsing in `SpriteSheetFactory`). There's no clear rule about when each approach applies.

### 14. Sprite accessor methods do redundant lookups

**File:** `src/graphics/sprite.cpp:98-128`

`GetCurrentFrameRect`, `GetCurrentFramePivot`, `GetWidth`, and `GetHeight` each independently call `m_spriteSheet->GetFrameDesc(GetCurrentFrame(), entry)`. In the common case where a caller needs both rect and pivot (e.g., the `DrawSprite` overloads), that's two separate map/slot lookups.

**Fix:** Add a `GetCurrentFrameEntry(FrameEntry&)` method, or return `std::optional<FrameEntry>`.

### 15. SDL text scratch texture grows monotonically

**File:** `src/graphics/sdl/sdl_graphics.cpp:293-296`

The scratch texture used for rotated text expands to fit the largest text ever drawn but never shrinks. Rendering one frame of full-window-width text permanently allocates a large texture.

### 16. Vulkan Font::WrapString has NOLINT for cognitive complexity

**File:** `src/graphics/vulkan/vulkan_font.cpp:286`

The word-wrapping function is 95 lines with a manually managed state machine (beginIdx, lastBreakIdx, i). It works but is hard to verify correct.

---

## Low Severity: API Nits and Improvements

### 17. Out-param API for GetFrameDesc / GetClipDesc

`SpriteSheet::GetFrameDesc(int, FrameEntry&)` and `GetClipDesc(string_view, ClipDesc&)` use mutable out-parameters instead of returning `std::optional<FrameEntry>` / `std::optional<ClipDesc>`. This forces callers to default-construct objects even when they only need to check existence.

### 18. No state query methods on IGraphics

There is no `GetBlendMode()`, `GetColor()`, or `GetCurrentClip()`. Debugging is harder without these, and the `MothRenderer` has to maintain parallel stacks (`m_drawColor`, `m_blendMode`, `m_clip`, `m_textureFilter`) purely because it can't read back from `IGraphics`.

### 19. SDL DrawToPNG hardcodes RGBA8888 surface format

**File:** `src/graphics/sdl/sdl_graphics.cpp:194`

If the source texture has a different format, `SDL_RenderReadPixels` may produce garbled output. There's no format query or conversion.

### 20. Vulkan Font destructor destroys potentially-null hb_buffer

**File:** `src/graphics/vulkan/vulkan_font.cpp:226-229`

```cpp
Font::~Font() {
    hb_font_destroy(m_hbFont);
    hb_buffer_destroy(m_hbBuffer);  // m_hbBuffer is nullptr until first ShapeString
}
```

Harfbuzz documents that `hb_buffer_destroy(nullptr)` is safe, so this is technically fine, but the asymmetry with `m_hbFont` (always non-null after construction) is worth a comment.

### 21. Ticker uses std::chrono::steady_clock::now() and sleep_for

**File:** `include/moth_graphics/utils/ticker.h`

The fixed-timestep loop is a textbook implementation. However, `sleep_for` can over-sleep due to OS scheduling jitter. A spin-wait or hybrid approach would provide tighter frame timing, though this is rarely an issue at 60 Hz.

### 22. FontFactory has no FlushCache / eviction

**File:** `src/graphics/font_factory.cpp`

Fonts are cached forever once loaded. `ClearFonts()` drops everything. There's no LRU eviction or per-font-size clearing. Not a problem for an editor loading a handful of fonts, but would leak in a long-running game that dynamically loads font sizes.

---

## Test Coverage Assessment

**Well-covered:**
- Sprite: construction, clip selection, playback, loop types, frame rect/pivot (`test_sprite.cpp`)
- SpriteSheet: frame/clip access, edge cases (`test_spritesheet.cpp`)
- Ticker: rate clamping, TickSync lifecycle (`test_ticker.cpp`)
- EventEmitter: pointer and lambda listeners, removal, handled propagation (`test_event_emitter.cpp`)

**API pinning only (compile-time, no runtime):**
- `api_surface_graphics.cpp` — IGraphics, IImage, IFont, ITarget signatures
- `api_surface_sprite.cpp` — SpriteSheet, Sprite signatures
- `api_surface_enums.cpp`, `api_surface_events.cpp`, `api_surface_platform.cpp`, `api_surface_moth_bridge.cpp`, `api_surface_umbrella.cpp`

**Missing entirely:**
- SpriteSheetFactory JSON parsing (no tests for any parse path or error case)
- ImageFactory texture pack loading and caching
- MothRenderer (bridge layer — no tests)
- MothFlipbook / MothImage / MothFont bridges
- Any Vulkan or SDL backend behavior (understandably requires GPU)

---

## Build System

- **CMake 3.10 minimum** — adequate for C++17. Could be bumped to 3.16+ for `FindPython` etc., but no pressing need.
- **PCH via `target_precompile_headers`** — good. `common.h` is comprehensive but guarded correctly for the SDL/Vulkan split.
- **`SKIP_PRECOMPILE_HEADERS`** — correctly applied to third-party sources.
- **`SKIP_LINTING`** — correctly applied to vendored/imgui/stb sources.
- **clang-tidy** — only enabled in Debug, good. The `-Wno-ignored-gch` workaround for GCC PCH is noted.
- **Resources (SPV files)** — not installed by the `install()` rules. If `moth_graphics` is consumed as a Conan package, the SPV shaders won't be present at runtime.
- **`#undef CreateWindow` in `common.h:41`** — necessary because Windows.h pollutes the namespace, but this is a Linux project. Fine as defensive hygiene.

---

## Summary

| Severity | Count | Key Items |
|----------|-------|-----------|
| Critical | 3 | Blend mode leak, bad_cast crash, context stack corruption |
| High | 6 | Monolith functions, dead code, empty base class, dual-meaning state, redundant filter calls, draw-vs-clear |
| Medium | 7 | Test duplication, symlink cache keys, hardcoded limits, error strategy, redundant lookups, texture growth, wrap complexity |
| Low | 6 | Out-param API, no state query, PNG format, hb_buffer, ticker jitter, font cache eviction |

The codebase is in reasonable shape for a pre-1.0 graphics library. The animation system (Sprite/SpriteSheet) is well-tested and clean. The Vulkan backend is competent but shows signs of having grown organically — the context stack, blend mode leak, and pipeline cache management deserve focused cleanup. The SDL backend is simpler and has fewer issues, but shares the `dynamic_cast` crash risk.

Highest-ROI fixes, in order:
1. Fix the Vulkan blend mode leak in `DrawText`
2. Change all backend `dynamic_cast<T&>` to `dynamic_cast<T*>` with null checks
3. Guard Vulkan `BeginContext` against double-push
4. Extract `SpriteSheetFactory::GetSpriteSheet` into sub-functions
5. Delete the commented-out code blocks
6. Use `VK_ATTACHMENT_LOAD_OP_CLEAR` for the swapchain render pass
