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

        virtual void NewFrame() = 0;
        virtual void Render(graphics::IGraphics& graphics) = 0;
        virtual void Shutdown() = 0;
    };

}
