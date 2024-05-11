#pragma once

#include "events/event_emitter.h"
#include "graphics/igraphics.h"
#include "layers/layer_stack.h"
#include "utils/vector.h"

namespace platform {
    class Window : public EventEmitter {
    public:
        Window(std::string const& windowTitle, int width, int height);
        virtual ~Window();

        virtual void Update(uint32_t ticks) {}
        virtual void Draw() {}

        virtual void SetWindowTitle(std::string const& title) = 0;

        int GetWidth() const { return m_windowWidth; }
        int GetHeight() const { return m_windowHeight; }
        graphics::IGraphics& GetGraphics() const { return *m_graphics; }
        LayerStack& GetLayerStack() const { return *m_layerStack; }

    protected:
        virtual bool CreateWindow() = 0;
        virtual void DestroyWindow() = 0;

        std::string m_title;
        int m_windowWidth = 0;
        int m_windowHeight = 0;
        IntVec2 m_windowPos = { -1, -1 };
        bool m_windowMaximized = false;
        std::unique_ptr<graphics::IGraphics> m_graphics;
        std::unique_ptr<LayerStack> m_layerStack;
    };
}
