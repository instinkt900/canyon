#pragma once

#include "graphics/context.h"

namespace platform {
    class Window;
    class IPlatform {
    public:
        virtual ~IPlatform() = default;

        virtual bool Startup() = 0;
        virtual void Shutdown() = 0;

        virtual graphics::Context& GetGraphicsContext() = 0;

        virtual std::unique_ptr<Window> CreateWindow(char const* title, int width, int height) = 0;
    };
}

