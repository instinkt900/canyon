#include "common.h"
#include <moth_ui/context.h>
#include <moth_ui/layers/layer.h>
#include "moth_graphics/platform/window.h"
#include "moth_graphics/graphics/surface_context.h"

namespace moth_graphics::platform {
    Window::Window(std::string_view title, int width, int height)
        : m_title(title)
        , m_windowWidth(width)
        , m_windowHeight(height) {
    }

    Window::~Window() {
    }

    graphics::ImageFactory& Window::GetImageFactory() const {
        return GetSurfaceContext().GetAssetContext().GetImageFactory();
    }

    bool Window::OnEvent(moth_ui::Event const& event) {
        // FireEvent from layers: dispatch to all layers first, then external
        // listeners if unhandled.
        moth_ui::EventDispatch dispatch(event);
        dispatch.Dispatch(m_layerStack.get());
        if (!dispatch.GetHandled()) {
            return EmitEvent(event);
        }
        return true;
    }

    void Window::PushLayer(std::unique_ptr<moth_ui::Layer> layer) {
        assert(m_layerStack && "PushLayer called before PostCreate; layer stack not yet initialised");
        m_layerStack->PushLayer(std::move(layer));
    }

    void Window::PostCreate() {
        auto& assetContext = GetSurfaceContext().GetAssetContext();
        m_uiRenderer = std::make_unique<moth_graphics::graphics::MothRenderer>(*m_graphics);
        m_mothImageFactory = std::make_unique<moth_graphics::graphics::MothImageFactory>(assetContext.GetImageFactory());
        m_mothFontFactory = std::make_unique<moth_graphics::graphics::MothFontFactory>(assetContext.GetFontFactory());
        m_mothFlipbookFactory = std::make_unique<moth_graphics::graphics::MothFlipbookFactory>(assetContext.GetSpriteSheetFactory());
        m_mothContext = std::make_shared<moth_ui::Context>(m_mothImageFactory.get(), m_mothFontFactory.get(), m_uiRenderer.get(), m_mothFlipbookFactory.get());
        m_layerStack = std::make_unique<moth_ui::LayerStack>(*m_uiRenderer, m_windowWidth, m_windowHeight, m_windowWidth, m_windowHeight);
        m_layerStack->SetEventListener(this);
    }

    void Window::PreDestroy() {
        m_uiRenderer.reset();
        m_mothImageFactory.reset();
        m_mothFontFactory.reset();
        m_mothFlipbookFactory.reset();
        m_mothContext.reset();
        m_layerStack.reset();
    }
}
