#pragma once

#include "graphics/vulkan/vulkan_context.h"
#include "platform/iplatform.h"
#include "platform/window.h"

#include <memory>

namespace platform::glfw {
    class Platform : public IPlatform {
    public:
        virtual ~Platform() = default;

        bool Startup() override;
        void Shutdown() override;

        graphics::Context& GetGraphicsContext() override;

        std::unique_ptr<platform::Window> CreateWindow(char const* title, int width, int height) override;

    private:
        std::unique_ptr<graphics::vulkan::Context> m_context;
    };
}
