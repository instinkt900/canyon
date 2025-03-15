#pragma once

#include "graphics/igraphics.h"
#include "graphics/sdl/sdl_context.h"
#include "graphics/sdl/sdl_graphics.h"
#include "platform/iplatform.h"

namespace platform::sdl {
    class Platform : public IPlatform {
    public:
        virtual ~Platform() = default;

        bool Startup() override;
        void Shutdown() override;

        graphics::Context& GetGraphicsContext() override;
        graphics::IGraphics& GetGraphics() override;

        std::unique_ptr<platform::Window> CreateWindow(char const* title, int width, int height) override;

    private:
        std::unique_ptr<graphics::sdl::Context> m_context = nullptr;
        std::unique_ptr<graphics::sdl::Graphics> m_graphics = nullptr;
    };
}

