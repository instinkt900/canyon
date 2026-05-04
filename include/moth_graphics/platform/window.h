#pragma once

#include "moth_graphics/events/event_emitter.h"
#include "moth_graphics/graphics/igraphics.h"
#include "moth_graphics/graphics/moth_ui/moth_flipbook_factory.h"
#include "moth_graphics/graphics/moth_ui/moth_font_factory.h"
#include "moth_graphics/graphics/moth_ui/moth_image_factory.h"
#include "moth_graphics/graphics/moth_ui/moth_renderer.h"
#include "moth_graphics/utils/vector.h"

#include <moth_ui/context.h>
#include <moth_ui/layers/layer_stack.h>

#include <memory>
#include <string>
#include <string_view>
#include <cassert>
#include <cstdint>

namespace moth_graphics::platform {
    class SurfaceContext;

    /// @brief A platform window and its associated rendering resources.
    ///
    /// Owns the @c IGraphics instance, the moth_ui @c LayerStack, and the
    /// @c MothImageFactory / @c MothFontFactory adapter wrappers. The underlying
    /// image and font factories are owned by the @c AssetContext. Subclasses
    /// handle the platform-specific window creation (see @c sdl::Window and
    /// @c glfw::Window).
    class Window : public EventEmitter, public moth_ui::IEventListener {
    public:
        /// @param windowTitle Initial title bar text.
        /// @param width Initial width in pixels.
        /// @param height Initial height in pixels.
        Window(std::string_view windowTitle, int width, int height);
        ~Window() override;

        /// @brief Poll events and advance the UI layer stack by @p ticks milliseconds.
        virtual void Update(uint32_t ticks) {}

        /// @brief Render one frame to this window.
        /// @return @c true if a frame was rendered, @c false if the backend skipped it.
        virtual bool Draw() { return false; }

        /// @brief Returns the per-window GPU resource context.
        virtual graphics::SurfaceContext& GetSurfaceContext() const = 0;

        /// @brief Update the window title bar text.
        virtual void SetWindowTitle(std::string_view title) = 0;

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

        /// @brief Returns the texture factory for this window.
        graphics::TextureFactory& GetTextureFactory() const;

        // -- IEventListener: receives FireEvent from LayerStack, dispatches to layers
        //    then rebroadcasts to external listeners via EmitEvent if unhandled.
        bool OnEvent(moth_ui::Event const& event) override;

        // Must be called after PostCreate (i.e. after the native window exists).
        void PushLayer(std::unique_ptr<moth_ui::Layer> layer);

    protected:
        /// @brief Returns the moth_ui layer stack. Only for internal/platform use.
        moth_ui::LayerStack& GetLayerStack() const { return *m_layerStack; }
        /// @brief Called after the native window and graphics objects are created.
        void PostCreate();

        /// @brief Called before the native window and graphics objects are destroyed.
        void PreDestroy();


        std::string m_title;
        int m_windowWidth = 0;
        int m_windowHeight = 0;
        IntVec2 m_windowPos = { -1, -1 };
        bool m_windowMaximized = false;

        std::unique_ptr<graphics::IGraphics> m_graphics;
        std::unique_ptr<moth_ui::LayerStack> m_layerStack;

        std::unique_ptr<graphics::MothImageFactory> m_mothImageFactory;
        std::unique_ptr<graphics::MothFontFactory> m_mothFontFactory;
        std::unique_ptr<graphics::MothFlipbookFactory> m_mothFlipbookFactory;
        std::unique_ptr<graphics::MothRenderer> m_uiRenderer;
        std::shared_ptr<moth_ui::Context> m_mothContext;
    };
}
