#pragma once

#include "platform/iplatform.h"

namespace platform::sdl {
    class Platform : public IPlatform {
    public:
        virtual ~Platform() = default;

        bool Startup() override;
        void Shutdown() override;

        std::unique_ptr<platform::Window> CreateWindow(char const* title, int width, int height) override;
    };
}

