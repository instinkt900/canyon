#pragma once

#include "moth_graphics/graphics/sdl/sdl_context.h"
#include "moth_graphics/platform/iplatform.h"
#include "moth_graphics/graphics/context.h"

#include <memory>

namespace moth_graphics::platform::sdl {
    class Platform : public IPlatform {
    public:
        ~Platform() override = default;

        bool Startup() override;
        void Shutdown() override;

        graphics::Context& GetGraphicsContext() override;

        std::unique_ptr<platform::Window> CreateWindow(std::string_view title, int width, int height) override;

        std::unique_ptr<ImGuiContext> CreateImGuiContext() override;

    private:
        std::unique_ptr<graphics::sdl::Context> m_context = nullptr;
    };
}

