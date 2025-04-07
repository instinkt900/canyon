#pragma once

#include "canyon/graphics/context.h"

namespace graphics::sdl {
    class Context : public graphics::Context {
    public:
        Context() = default;
        virtual ~Context() = default;
    };
}
