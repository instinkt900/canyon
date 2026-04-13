// SDL2main.lib (Windows) provides WinMain which calls SDL_main.
// Catch2WithMain owns main and does not define SDL_main.
// This stub satisfies the unresolved SDL_main reference by forwarding to main,
// so Catch2's entry point is invoked via SDL's WinMain without renaming it.
int main(int argc, char* argv[]);
extern "C" int SDL_main(int argc, char* argv[]) {
    return main(argc, argv);
}
