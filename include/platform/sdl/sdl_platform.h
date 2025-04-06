#pragma once

#include "graphics/sdl/sdl_context.h"
#include "platform/iplatform.h"

#include <memory>

namespace platform::sdl {
    class Platform : public IPlatform {
    public:
        virtual ~Platform() = default;

        bool Startup() override;
        void Shutdown() override;

        graphics::Context& GetGraphicsContext() override;

        std::unique_ptr<platform::Window> CreateWindow(char const* title, int width, int height) override;

    private:
        std::unique_ptr<graphics::sdl::Context> m_context = nullptr;
    };
}

