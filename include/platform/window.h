#pragma once

#include "events/event_emitter.h"
#include "graphics/font_factory.h"
#include "graphics/igraphics.h"
#include "graphics/image_factory.h"
#include "layers/layer_stack.h"
#include "utils/vector.h"
#include "graphics/moth_ui/moth_font_factory.h"
#include "graphics/moth_ui/moth_image_factory.h"
#include "graphics/moth_ui/moth_renderer.h"

namespace platform {
    class Window : public EventEmitter {
    public:
        Window(std::string const& windowTitle, int width, int height);
        virtual ~Window();

        virtual void Update(uint32_t ticks) {}
        virtual void Draw() {}

        virtual graphics::SurfaceContext& GetSurfaceContext() const = 0;
        virtual void SetWindowTitle(std::string const& title) = 0;

        int GetWidth() const { return m_windowWidth; }
        int GetHeight() const { return m_windowHeight; }
        graphics::IGraphics& GetGraphics() const { return *m_graphics; }
        LayerStack& GetLayerStack() const { return *m_layerStack; }

    protected:
        void Finalize();
        virtual bool CreateWindow() = 0;
        virtual void DestroyWindow() = 0;

        std::string m_title;
        int m_windowWidth = 0;
        int m_windowHeight = 0;
        IntVec2 m_windowPos = { -1, -1 };
        bool m_windowMaximized = false;

        std::unique_ptr<graphics::IGraphics> m_graphics;
        std::unique_ptr<LayerStack> m_layerStack;

        std::shared_ptr<graphics::ImageFactory> m_imageFactory;
        std::shared_ptr<graphics::FontFactory> m_fontFactory;

        std::unique_ptr<graphics::MothImageFactory> m_mothImageFactory;
        std::unique_ptr<graphics::MothFontFactory> m_mothFontFactory;
        std::unique_ptr<graphics::MothRenderer> m_uiRenderer;
    };
}
