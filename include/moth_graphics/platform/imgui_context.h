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
        virtual void Shutdown() = 0;
    };

}
