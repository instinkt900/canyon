#include "common.h"
#include <moth_ui/context.h>
#include "canyon/platform/window.h"

namespace canyon::platform {
    Window::Window(std::string const& title, int width, int height)
        : m_title(title)
        , m_windowWidth(width)
        , m_windowHeight(height) {
    }

    Window::~Window() {
    }

    void Window::PostCreate() {
        auto& surfaceContext = GetSurfaceContext();
        m_imageFactory = std::make_unique<canyon::graphics::ImageFactory>(surfaceContext);
        m_fontFactory = std::make_unique<canyon::graphics::FontFactory>(surfaceContext);
        m_uiRenderer = std::make_unique<canyon::graphics::MothRenderer>(*m_graphics);
        m_mothImageFactory = std::make_unique<canyon::graphics::MothImageFactory>(m_imageFactory);
        m_mothFontFactory = std::make_unique<canyon::graphics::MothFontFactory>(m_fontFactory);
        m_mothContext = std::make_shared<moth_ui::Context>(m_mothImageFactory.get(), m_mothFontFactory.get(), m_uiRenderer.get());
        m_layerStack = std::make_unique<moth_ui::LayerStack>(*m_uiRenderer, m_windowWidth, m_windowHeight, m_windowWidth, m_windowHeight);
    }

    void Window::PreDestroy() {
        m_imageFactory.reset();
        m_fontFactory.reset();
        m_uiRenderer.reset();
        m_mothImageFactory.reset();
        m_mothFontFactory.reset();
        m_mothContext.reset();
        m_layerStack.reset();
    }
}
