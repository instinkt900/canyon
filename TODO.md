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

## Documentation

**Effort:** Small

Two gaps in the README:
- **Known limitations:** document current constraints so consumers are not caught off guard.
- **API stability statement:** clarify whether the public API (`IGraphics`, `SurfaceContext`,
  `Application`, etc.) is considered stable at 1.0 and what the compatibility policy is going
  forward.
