#pragma once

#include "canyon/events/event_emitter.h"
#include "canyon/graphics/font_factory.h"
#include "canyon/graphics/igraphics.h"
#include "canyon/graphics/image_factory.h"
#include "canyon/graphics/moth_ui/moth_font_factory.h"
#include "canyon/graphics/moth_ui/moth_image_factory.h"
#include "canyon/graphics/moth_ui/moth_renderer.h"
#include "canyon/utils/vector.h"

#include <moth_ui/context.h>
#include <moth_ui/layers/layer_stack.h>

#include <memory>
#include <string>
#include <cstdint>

namespace canyon::platform {
    class SurfaceContext;

    /// @brief A platform window and its associated rendering resources.
    ///
    /// Owns the @c IGraphics instance, the moth_ui @c LayerStack, and the asset
    /// factories for images and fonts. Subclasses handle the platform-specific
    /// window creation (see @c sdl::Window and @c glfw::Window).
    class Window : public EventEmitter {
    public:
        /// @param windowTitle Initial title bar text.
        /// @param width Initial width in pixels.
        /// @param height Initial height in pixels.
        Window(std::string const& windowTitle, int width, int height);
        ~Window() override;

        /// @brief Poll events and advance the UI layer stack by @p ticks milliseconds.
        virtual void Update(uint32_t ticks) {}

        /// @brief Render one frame to this window.
        virtual void Draw() {}

        /// @brief Returns the per-window GPU resource context.
        virtual graphics::SurfaceContext& GetSurfaceContext() const = 0;

        /// @brief Update the window title bar text.
        virtual void SetWindowTitle(std::string const& title) = 0;

        /// @brief Returns @c true if the window is currently maximized.
        bool IsMaximized() const { return m_windowMaximized; }

        /// @brief Returns the current window position in screen coordinates.
        IntVec2 const& GetPosition() const { return m_windowPos; }

        /// @brief Returns the current window width in pixels.
        int GetWidth() const { return m_windowWidth; }

        /// @brief Returns the current window height in pixels.
        int GetHeight() const { return m_windowHeight; }

        /// @brief Returns the moth_ui context used for UI rendering.
        moth_ui::Context& GetMothContext() const { return *m_mothContext; }

        /// @brief Returns the graphics interface for this window.
        graphics::IGraphics& GetGraphics() const { return *m_graphics; }

        /// @brief Returns the moth_ui layer stack for this window.
        moth_ui::LayerStack& GetLayerStack() const { return *m_layerStack; }

    protected:
        /// @brief Called after the native window and graphics objects are created.
        void PostCreate();

        /// @brief Called before the native window and graphics objects are destroyed.
        void PreDestroy();

        /// @brief Create the native window and initialise graphics resources.
        virtual bool CreateWindow() = 0;

        /// @brief Destroy the native window and release graphics resources.
        virtual void DestroyWindow() = 0;

        std::string m_title;
        int m_windowWidth = 0;
        int m_windowHeight = 0;
        IntVec2 m_windowPos = { -1, -1 };
        bool m_windowMaximized = false;

        std::unique_ptr<graphics::IGraphics> m_graphics;
        std::unique_ptr<moth_ui::LayerStack> m_layerStack;

        std::shared_ptr<graphics::ImageFactory> m_imageFactory;
        std::shared_ptr<graphics::FontFactory> m_fontFactory;

        std::unique_ptr<graphics::MothImageFactory> m_mothImageFactory;
        std::unique_ptr<graphics::MothFontFactory> m_mothFontFactory;
        std::unique_ptr<graphics::MothRenderer> m_uiRenderer;
        std::shared_ptr<moth_ui::Context> m_mothContext;
    };
}
