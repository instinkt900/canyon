#pragma once

#include "canyon/graphics/vulkan/vulkan_context.h"
#include "canyon/graphics/context.h"
#include "canyon/platform/iplatform.h"
#include "canyon/platform/window.h"

#include <memory>

namespace canyon::platform::glfw {
    class Platform : public IPlatform {
    public:
        virtual ~Platform() = default;

        bool Startup() override;
        void Shutdown() override;

        canyon::graphics::Context& GetGraphicsContext() override;

        std::unique_ptr<canyon::platform::Window> CreateWindow(char const* title, int width, int height) override;

    private:
        std::unique_ptr<canyon::graphics::vulkan::Context> m_context;
    };
}
