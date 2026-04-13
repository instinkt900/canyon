// Test session entry point.
//
// On Windows, SDL.h #defines main → SDL_main (unless SDL_MAIN_HANDLED is set).
// SDL2main.lib's WinMain parses the process command line and calls SDL_main,
// so this function is invoked correctly with argc/argv by the Windows entry point.
//
// On Linux/macOS, SDL does not redefine main; the CRT calls this function directly.
//
// Using Catch2::Catch2 (not Catch2WithMain) so we control the entry point here
// and SDL's renaming applies to it.
#include <SDL.h>
#include <catch2/catch_session.hpp>

int main(int argc, char* argv[]) {
    return Catch::Session().run(argc, argv);
}
