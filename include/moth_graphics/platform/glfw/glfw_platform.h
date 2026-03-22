#pragma once

#include "moth_graphics/graphics/vulkan/vulkan_context.h"
#include "moth_graphics/graphics/context.h"
#include "moth_graphics/platform/iplatform.h"
#include "moth_graphics/platform/window.h"

#include <memory>

namespace moth_graphics::platform::glfw {
    class Platform : public IPlatform {
    public:
        virtual ~Platform() = default;

        bool Startup() override;
        void Shutdown() override;

        moth_graphics::graphics::Context& GetGraphicsContext() override;

        std::unique_ptr<moth_graphics::platform::Window> CreateWindow(std::string const& title, int width, int height) override;

    private:
        std::unique_ptr<moth_graphics::graphics::vulkan::Context> m_context;
    };
}
