# TODO

Issues identified during code review. Work through each by verifying against current code before fixing.

---

## SDL Multi-Window Input Routing

Input events (keyboard, mouse, etc.) are collected via a shared static
`PendingEvents` list in `sdl_window.cpp`. Each call to
`CollectSDLEventsForWindow()` polls SDL, dumps everything into the shared list,
then extracts events matching the calling window's ID and removes them.

The problem: input events have `windowID == 0` (or the focused window's ID) and
are matched and removed by whichever window calls `Update()` first. In a
multi-window setup the second window never sees keyboard/mouse input.

Proposed fix: route input events to the window that currently has SDL focus.
`SDL_GetKeyboardFocus()` and `SDL_GetMouseFocus()` return the focused
`SDL_Window*`; use `SDL_GetWindowID()` on that to resolve the correct target
before dispatching. Alternatively, deliver input events to all windows and let
each decide relevance — consistent with how most windowing systems broadcast
input to the foreground window only.

---

## SurfaceContext Design

The current split — `Context` (application-wide, holds Vulkan instance / SDL
init) and `SurfaceContext` (per-window, holds renderer, pools, allocators) — is
mostly right but the boundary is fuzzy. The factories (`ImageFactory`,
`FontFactory`) live on `SurfaceContext`, which is correct because assets are
window-bound, but consumers sometimes reach through to `GetContext()` to get
application-wide resources.

From NOTES (29/03/25): the intent was `Context` = application context,
`SurfaceContext` = surface/window context, and an `AssetLoader` that comes from
`SurfaceContext` for disk I/O. The asset-loading methods (`ImageFromFile`,
`FontFromFile`, `TextureFromFile`) currently live directly on `SurfaceContext`
rather than on a dedicated loader type.

Options:
- **Option A — Keep current shape, clarify the interface**: Document that
  `SurfaceContext` is intentionally the asset entry point; remove
  `GetContext()` from the public API so callers can't reach through.
- **Option B — Extract `AssetLoader`**: `SurfaceContext` becomes purely about
  GPU resource lifetime; a separate `AssetLoader` (obtained from
  `SurfaceContext`) handles file I/O and caching. Cleaner separation but more
  surface area.

---

## Test Suite

There are no automated tests. The example app exercises the library manually but
is not a regression harness. Without tests it is difficult to verify correctness
across backends or catch regressions when making changes.

Suggested scope for a 1.0 test suite:

- **Smoke tests**: platform startup/shutdown, window creation and destruction,
  basic draw calls through both backends without crashing.
- **Asset loading**: texture and font load from file; verify dimensions/metrics
  are as expected.
- **Layer stack**: push/pop layers, verify update and draw ordering.
- **Event routing**: emit window events, verify listeners receive them; verify
  `SDL_QUIT` propagates correctly.

A lightweight framework (Catch2 or GoogleTest) can be added as a Conan dev
dependency without affecting the shipped package. Backends that require a
display can be guarded with a CMake option for headless CI.

---

## Shared Library Support

`canyon.h` already has the `CANYON_API` export macro defined correctly for both Windows (`__declspec(dllexport/dllimport)`) and Linux (`__attribute__((visibility("default")))`), but it is currently unused and the build is locked to static.

To enable shared library support:

1. **`CMakeLists.txt`** — move `CANYON_BUILD` out of the `if(MSVC)` block so it is defined on all platforms; without it the Linux visibility macro never triggers.
2. **`conanfile.py`** — change `package_type = "static-library"` to `package_type = "library"` and add a `shared` option.
3. **Public headers** — annotate every public class declaration with `CANYON_API` across all headers in `include/canyon/` (currently zero classes are annotated). This is the bulk of the work.

Note: changing these signatures is a **public API and ABI break** — all consumers (moth_editor, any downstream) must be recompiled. Coordinate with a version bump in `version.txt`.

---

## Documentation

- **Known limitations**: the README covers usage but does not mention the vertex
  buffer limit or any other current constraints. A brief "Known Limitations"
  section prevents surprises for new consumers.
- **API stability statement**: clarify in the README whether the public API
  (`IGraphics`, `SurfaceContext`, `Application`, etc.) is considered stable at
  1.0, and what the compatibility policy is going forward.
