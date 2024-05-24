#pragma once

#include "platform/window.h"

namespace platform {
    class IPlatform {
    public:
        virtual ~IPlatform() = default;

        virtual bool Startup() = 0;
        virtual void Shutdown() = 0;

        virtual std::unique_ptr<Window> CreateWindow(char const* title, int width, int height) = 0;
    };
}

