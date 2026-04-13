// SDL2main.lib (Windows) provides WinMain which references SDL_main.
// In a console-subsystem test binary (forced via /SUBSYSTEM:CONSOLE), WinMain is
// never called, so SDL_main is never invoked. This stub satisfies the linker
// reference left by SDL_windows_main.obj; the CRT calls Catch2's main directly.
extern "C" int SDL_main(int /*argc*/, char* /*argv*/[]) { return 0; }
