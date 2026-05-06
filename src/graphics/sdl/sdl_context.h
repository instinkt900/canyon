#pragma once

#include "moth_graphics/graphics/context.h"

namespace moth_graphics::graphics::sdl {
    class Context : public moth_graphics::graphics::Context {
    public:
        Context() = default;
        ~Context() override = default;

        bool Startup() override { return true; }
        void Shutdown() override {}
    };
}
