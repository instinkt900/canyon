# TODO

## Test Suite

**Effort:** Medium

No automated tests exist. The example app exercises the library manually but is not a regression
harness. Without tests it is difficult to verify correctness across backends or catch regressions
when making changes.

Suggested scope:
- **Smoke tests:** platform startup/shutdown, window creation and destruction, basic draw calls
  through both backends without crashing.
- **Asset loading:** texture and font load from file; verify dimensions and metrics are as
  expected.
- **Layer stack:** push/pop layers, verify update and draw ordering.
- **Event routing:** emit window events, verify listeners receive them; verify `SDL_QUIT`
  propagates correctly.

A lightweight framework (Catch2 or GoogleTest) can be added as a Conan dev dependency without
affecting the shipped package. Backends requiring a display can be guarded with a CMake option
for headless CI.

---

## Shared Library Support

**Effort:** Large

`CANYON_API` export macros are already defined correctly for both Windows
(`__declspec(dllexport/dllimport)`) and Linux (`__attribute__((visibility("default")))`), but
are currently unused and the build is locked to static.

Steps required:
1. **`CMakeLists.txt`** — move `CANYON_BUILD` out of the `if(MSVC)` block so it is defined on
   all platforms; without it the Linux visibility macro never triggers.
2. **`conanfile.py`** — change `package_type = "static-library"` to `package_type = "library"`
   and add a `shared` option.
3. **Public headers** — annotate every public class declaration with `CANYON_API` across all
   headers in `include/canyon/`. This is the bulk of the work.

Note: this is a public API and ABI break — all consumers must be recompiled. Coordinate with a
version bump in `version.txt`.

---

## Documentation

**Effort:** Small

Two gaps in the README:
- **Known limitations:** document current constraints so consumers are not caught off guard.
- **API stability statement:** clarify whether the public API (`IGraphics`, `SurfaceContext`,
  `Application`, etc.) is considered stable at 1.0 and what the compatibility policy is going
  forward.
