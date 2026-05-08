# moth_graphics 1.0 Readiness Review

**Reviewed at:** version `1.0.0-rc.1`, branch `master`. RC has been baking ~1 week.
**Goal:** confirm the API is in good shape and won't lock the project into anything regrettable in 1.x.

The library is in genuinely good shape — the prior tech-review work landed cleanly, the API surface tests pin signatures, sticky-setter `IGraphics` is solid, the `Image`/`Sprite` value types feel right, and the ImGui split out of `IGraphics` was the right move. The biggest 1.0 risks are **scope of public surface**, not bad shapes.

Findings are grouped by lock-in cost.

---

## Blocking — would commit you to things you'll regret in 1.x

### 1. The entire Vulkan implementation is in the public include path

~895 lines of Vulkan internals are under `include/`:

- `vulkan_buffer.h`, `vulkan_command_buffer.h`, `vulkan_pipeline.h`, `vulkan_renderpass.h`, `vulkan_shader.h`, `vulkan_swapchain.h`, `vulkan_framebuffer.h`, `vulkan_fence.h`, `vulkan_utils.h`
- Builders: `PipelineBuilder`, `RenderPassBuilder`, `ShaderBuilder` — full mutable state of how pipelines are constructed
- `Pipeline`'s members are *all public* (`m_hash`, `m_device`, `m_pipeline`, `m_shader`)
- `Shader` is a `struct`, not a class — every member public
- `Texture` exposes `id`, `m_vkExtent`, `m_vkSamplerLinear`, `m_vkSamplerNearest`, `m_vkDescriptorSet`, `m_addressModeU`, etc. as protected
- `vulkan_utils.h` exports a `CHECK_VK_RESULT` macro that calls `abort()` — public macro that can crash the host

Shipping any of this in 1.0 commits you to ABI/API stability for it. **Recommend:** move to `src/` as private headers. Keep public only what's needed for extension points: `vulkan_context.h`, `vulkan_surface_context.h`, `vulkan_graphics.h` (slimmed down — see #2), `vulkan_asset_context.h`, and arguably `vulkan_image.h`/`vulkan_texture.h` if `dynamic_cast` from `IImage`/`ITexture` is a documented user pattern.

### 2. `vulkan::Graphics`/`vulkan::Font` leak HarfBuzz/FreeType/internal types

`vulkan_graphics.h` publicly exposes `Vertex`, `FontRect`, `FontGlyphInstance`, the `DrawContext` struct, `GetSwapchain()`, `GetRenderPass()`, `GetCurrentCommandBuffer()`, `GetDescriptorSet(Texture&)`, `GetFontShader()`, `OnResize(...)`. None of these are required for the `IGraphics` contract.

`vulkan_font.h` publicly exposes `hb_font_t*`, `hb_buffer_t*`, `FT_Face`, `ShapedInfo`, `LineDesc`, `GetVKDescriptorSetForShader`. Pulling `<harfbuzz/hb.h>` into every consumer is heavy *and* turns harfbuzz/freetype version into part of your ABI.

### 3. SDL backend leaks SDL into all consumers via the public `smart_sdl.hpp`

`include/.../sdl/smart_sdl.hpp` includes `<SDL_image.h>`, `<SDL_pixels.h>`, `<SDL_render.h>`, `<SDL_surface.h>`, `<SDL_ttf.h>`. Anyone who pulls in `sdl_graphics.h` (or anything that transitively includes it — `sdl_texture.h`, `sdl_target.h`, `sdl_font.h`, `sdl_asset_context.h`) drags the whole SDL+SDL_ttf+SDL_image surface in.

The vendored `SDL_FontCache.h` is also publicly installed. It's an internal vendoring decision that becomes a public dependency at 1.0.

### 4. `dynamic_cast<T&>` (throwing form) still in 4 hot paths

The earlier review flagged these; they're still there:

- `sdl_graphics.cpp:174` — `dynamic_cast<Font&>(font)` in `DrawText`
- `vulkan_graphics.cpp:276` — `dynamic_cast<Font&>(font)` in `DrawText`
- `sdl_platform.cpp:22` — `dynamic_cast<sdl::Window&>(window)` in `ImGuiContext::Init`
- `glfw_platform.cpp:53,101` — `dynamic_cast<vulkan::Graphics&>(graphics)` in `ImGuiContext::Init`/`Render`

If a consumer crosses streams (passes a wrong-backend object), these throw `std::bad_cast` instead of a logged-and-skipped no-op. The pointer-and-null-check pattern is already in place at `moth_renderer.cpp:120,150` and `sdl_graphics.cpp:279` — match it everywhere.

### 5. README example code does not compile against the current API

```cpp
gfx.DrawImage(*image, destRect);                  // OK only if image is unique_ptr<Image>
gfx.DrawImage(*target->GetImage(), destRect);     // GetImage() now returns Image by value — not deref-able
```

Anyone writing 1.0-day code from the README hits a wall. This is small to fix but ships with 1.0.

### 6. Tick loop can leak an open ImGui frame

`Application::Tick` calls `m_imguiContext->NewFrame()` unconditionally, then only calls `Render` if `m_window->Draw()` returned true. If `Draw()` returns false (Vulkan swapchain out-of-date — *the documented legitimate case*), the next `NewFrame()` runs against an unclosed frame. ImGui requires `Render()` *or* `EndFrame()`; the `ImGuiContext` interface exposes neither for the skip path.

Either: gate `NewFrame()` on `Draw()` succeeding (delay it), or add an `EndFrameSkipped()` hook to `ImGuiContext` and call it on the false branch.

---

## High — would prefer to fix before 1.0

### 7. File name vs. type name mismatch

`include/.../graphics/iimage.h` contains `class Image` (a value type, not an interface). Same drift around `itarget.h`. Keeping the `i` prefix on a non-interface header is a long-term gotcha for both readers and grep. Renaming after 1.0 is API-breaking for any consumer that includes the file directly. Rename now: `iimage.h` → `image.h`.

### 8. Umbrella header is incomplete

`moth_graphics.h` is missing:
- `graphics/sprite.h`, `graphics/spritesheet.h`, `graphics/spritesheet_factory.h`
- `graphics/moth_ui/moth_flipbook.h`, `moth_flipbook_factory.h`
- `platform/imgui_context.h`
- `utils/transform.h` (only transitively pulled in by `igraphics.h`)
- The `EventRender*` types are defined in `events/event_window.h` but not advertised by name

Consumers including only the umbrella header miss public types. Lock-in is mild (they can include the missing headers explicitly), but the *promise* of an umbrella header is "everything public lives here."

### 9. `graphics/moth_ui/utils.h` is 80 lines of commented-out conversion code

The earlier review flagged commented-out blocks in headers; this whole file is one. Either delete it or restore the conversions if you actually need them. Shipping it as-is signals "we never finished this."

### 10. `SpriteSheetFactory::GetSpriteSheet` is still a 180-line monolith

The earlier review item is unresolved — `NOLINTBEGIN(readability-function-cognitive-complexity)` is still there. Not a lock-in issue per se, but the function is the canonical reference implementation for `.flipbook.json` parsing — the format being baked into 1.0 makes the parser quality matter.

### 11. `Window::Update` and `Window::Draw` are not pure virtual

```cpp
virtual void Update(uint32_t ticks) {}
virtual bool Draw() { return false; }
```

Every concrete implementation overrides them. The default-do-nothing-and-return-false makes a misconfigured `Window` silently inert. `= 0` is honest; nobody constructs a base `Window` directly anyway.

### 12. `Window` exposes ownership of every owned resource as `protected:`

```cpp
protected:
    std::unique_ptr<graphics::IGraphics> m_graphics;
    std::unique_ptr<moth_ui::LayerStack> m_layerStack;
    std::unique_ptr<graphics::MothImageFactory> m_mothImageFactory;
    // ... etc
```

Subclasses can null/replace any of these and break invariants. With `PostCreate()`/`PreDestroy()` being subclass-called manual lifecycle, this is a fragile contract for a 1.0 stable interface. Recommend: move to `private:` and add protected accessors only for what subclasses legitimately need.

### 13. `Application::SetImGuiViewportsEnabled(bool)` after `Init()` silently does nothing

The doc string says "Must be called before Init()." Silent footguns are bad in a 1.0 interface. Three options:
- Make it a constructor parameter (cleanest, breaks current API to fix)
- Assert when called after `Init`
- Make it work at runtime (probably big and not worth it)

### 14. `MothImageFactory` adds non-virtual public methods

`MothImageFactory` inherits `moth_ui::IImageFactory` (which has only `GetImage` virtual) and adds `FlushCache()` and `LoadTexturePack()` as plain non-virtual methods. They only work through the concrete type. A consumer who has `moth_ui::IImageFactory&` can't access them. Either:
- Move them onto `moth_ui::IImageFactory` (probably wrong layer — moth_ui shouldn't know about texture packs)
- Make consumers reach for them through `Window::GetTextureFactory()` instead, and remove from `MothImageFactory`

### 15. `graphics::FontFactory::GetFont(name, size)` "name" is actually a path

```cpp
std::shared_ptr<IFont> GetFont(std::string const& name, int size);  // "name = Path to the font file (TTF/OTF)"
```

Misleading parameter name. Either rename to `path`, or accept *both* (named registration + raw path) and pick a single mental model. Right now you have two factories with two different meanings of "name": `moth_ui::FontFactory::GetFont(name, …)` looks up a registered name; `graphics::FontFactory::GetFont(name, …)` interprets it as a path. `MothFontFactory` translates between them but the asymmetry is locked in by 1.0.

Bonus: `FontFactory::GetFont` caches the result of a *failed* `FontFromFile` (it stores the null in the cache). One bad load → permanent failure for that path. Add a null-check before insert.

---

## Medium — defer with awareness

### 16. `AssetContext::TextureFromPixels` only accepts RGBA

Tightly defined ("4 bytes per pixel: R, G, B, A"). Adding grayscale/RGB later is an additive change. Just be aware that 1.0 commits to RGBA-only at this entry point.

### 17. `EventEmitter::EmitEvent` is O(n²)

(Deferred by design per earlier review.) If you have any consumer with >50 listeners, this will show in profiles. Document at least.

### 18. `Ticker::m_running` is `std::atomic<bool>` but the loop is single-threaded

`TickSync()` writes from the running thread; `SetRunning(false)` is presumably called from event handlers on the same thread. The `atomic` advertises thread safety you don't actually support. Either drop the atomic, or document that external `SetRunning(false)` from another thread is supported.

### 19. SDL `Texture::SetAddressMode` is silently a no-op

```cpp
void SetAddressMode(...) override {} // not supported in SDL2
```

Locked into `ITexture` for 1.0. SDL2 backend will silently fail to apply address modes. At minimum, log once at warn level.

### 20. `Window::PostCreate()` / `PreDestroy()` two-step construction is fragile

Subclasses must call these in their ctor/dtor. Forgetting `PostCreate()` leaves `m_layerStack` null and `PushLayer` asserts. Forgetting `PreDestroy()` leaks. Standard CRTP or factory pattern would close this; not blocking 1.0.

### 21. `Sprite` has explicitly-defaulted copy/move/assignment

Redundant — no special members declared, defaults would be generated. Harmless but signals over-engineering.

### 22. `vulkan_graphics_pipeline.cpp:155` has a stale `// TODO this might have to change?`

Uncommitted decision (`VK_FORMAT_R8G8B8A8_SRGB` vs the chosen format). For 1.0, either decide and remove the TODO, or document why it's deferred.

### 23. Backend `Pipeline`/`Shader` exposing all members publicly

Reiterating from #1: these aren't even encapsulated within the implementation. If you keep them public per the recommendation in #1, fix the encapsulation.

---

## Low / nits

- `TextureFactory::TextureDesc` has inconsistent field naming: `m_texture`, `m_path`, but `sourceRect` (no prefix).
- `vulkan::Texture::id` (no prefix) vs `m_vkExtent`/`m_vkFormat`. Same struct, different conventions.
- `utils/rect_format.h` is now a one-line forwarder to `moth_ui_format.h`. Either remove or accept the shim cost.
- `version.h.in` is configured into `${CMAKE_CURRENT_SOURCE_DIR}/include/...` (CMakeLists.txt:9). Build output written into the source tree means dirty checkouts after a configure. Configure into the build dir and add it to the include path.
- `MothFontFactory::GetFont` requires prior `AddFont(name, path)` (registration in base class) — silent nullptr otherwise. Document or assert.
- `IPlatform::CreateImGuiContext()` factory could be replaced by an `ImGuiContext` constructor taking the platform; this is post-1.0 ergonomics.

---

## Tests pin most of the right surface

`api_surface_*.cpp` tests do an excellent job locking method signatures, enum values, and inheritance for IGraphics/Image/IFont/ITarget/Application/Window/IPlatform/Ticker/MothRenderer/MothImage/MothImageFactory/MothFontFactory/SpriteSheet/Sprite/EventEmitter/EventWindowSize. This is exactly the right thing to ship at 1.0. Things you may want to add pins for:

- `ImGuiContext` (currently no pin)
- `EventRenderDeviceReset`/`EventRenderTargetReset` static-type values
- `LambdaHandle` (only `IsValid` is pinned)

---

## Recommendation for 1.0

**Do not ship 1.0 with the current public Vulkan headers.** Items 1–6 are the ones that will bite you for years: the rest are recoverable in 1.x with deprecations. Concretely:

1. Move all `vulkan_*.h` except `vulkan_context.h`, `vulkan_surface_context.h`, `vulkan_asset_context.h`, `vulkan_graphics.h` (slimmed), `vulkan_image.h`/`vulkan_texture.h` (if extension is wanted) into `src/`.
2. Slim `vulkan_graphics.h`: remove `Vertex`, `FontRect`, `FontGlyphInstance`, the internal getters, and `OnResize` from the public surface. Keep only the `IGraphics` overrides and what consumers must reach.
3. Stop publishing `smart_sdl.hpp` and `SDL_FontCache.h` from the include directory (move to `src/`). Same review for `sdl_*.h` — consider what's truly needed for extension.
4. Fix the four throwing `dynamic_cast<T&>` sites (#4).
5. Fix the README examples (#5).
6. Fix the open-ImGui-frame bug in `Tick` (#6).

Items 7–15 (file rename, umbrella, dead utils.h, etc.) are quick wins; bundle them into one PR. Everything from 16 down can wait for 1.x with a clear changelog note.

Once those land, `1.0.0-rc.2` and the API will be in a place where you're committing only to shapes you'd actually keep.
