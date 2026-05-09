#pragma once

namespace moth_graphics::graphics::sdl {
    /// @brief SDL graphics context.
    ///
    /// Created and managed by the SDL platform backend. Startup() and
    /// Shutdown() are called by the platform during its own lifecycle.
    class Context {
    public:
        bool Startup() { return true; }
        void Shutdown() {}
    };
}
