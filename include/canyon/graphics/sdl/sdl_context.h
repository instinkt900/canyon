#pragma once

#include "canyon/graphics/context.h"

namespace canyon::graphics::sdl {
    class Context : public canyon::graphics::Context {
    public:
        Context() = default;
        virtual ~Context() = default;
    };
}
