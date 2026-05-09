#pragma once

#include "moth_graphics/graphics/context.h"
#include "moth_graphics/platform/iplatform.h"
#include "moth_graphics/platform/window.h"

#include <memory>

namespace moth_graphics::graphics::vulkan {
    class Context;
}

namespace moth_graphics::platform::glfw {
    class Platform : public IPlatform {
    public:
        Platform();
        ~Platform() noexcept override;

        bool Startup() override;
        void Shutdown() override;

        moth_graphics::graphics::Context& GetGraphicsContext() override;

        std::unique_ptr<moth_graphics::platform::Window> CreateWindow(std::string_view title, int width, int height) override;

        std::unique_ptr<ImGuiContext> CreateImGuiContext(platform::Window& window, graphics::IGraphics& graphics, bool enableViewports) override;

    private:
        void ShutdownImpl();

        std::unique_ptr<moth_graphics::graphics::vulkan::Context> m_context = nullptr;
        bool m_initialized = false;
    };
}
