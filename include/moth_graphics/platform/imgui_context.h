#pragma once

namespace moth_graphics {
    namespace platform {
        class Window;
    }
    namespace graphics {
        class IGraphics;
    }
}

namespace moth_graphics::platform {

    class ImGuiContext {
    public:
        virtual ~ImGuiContext() = default;

        virtual bool Init(Window& window, graphics::IGraphics& graphics, bool enableViewports) = 0;
        virtual void NewFrame() = 0;
        virtual void Render(graphics::IGraphics& graphics) = 0;

        /// @brief Discard the in-progress ImGui frame without rendering it.
        ///
        /// Called when the host can't draw the current frame (e.g. Vulkan
        /// swapchain out-of-date). Closes the frame opened by @c NewFrame so
        /// the next @c NewFrame doesn't run against an unclosed frame.
        virtual void DiscardFrame() = 0;

        virtual void Shutdown() = 0;
    };

}
