#pragma once

#include "moth_graphics/events/event_emitter.h"
#include "moth_graphics/graphics/igraphics.h"
#include "moth_graphics/graphics/moth_ui/moth_flipbook_factory.h"
#include "moth_graphics/graphics/moth_ui/moth_font_factory.h"
#include "moth_graphics/graphics/moth_ui/moth_image_factory.h"
#include "moth_graphics/graphics/moth_ui/moth_renderer.h"
#include "moth_graphics/platform/imgui_context.h"
#include "moth_graphics/utils/vector.h"

#include <moth_ui/context.h>
#include <moth_ui/layers/layer_stack.h>

#include <memory>
#include <string>
#include <string_view>
#include <cassert>
#include <cstdint>

namespace moth_graphics::graphics {
    class SurfaceContext;
}

namespace moth_graphics::platform {
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
        virtual void Update(uint32_t ticks) = 0;

        /// @brief Begin rendering one frame.
        virtual void BeginFrame() = 0;

        /// @brief Finish rendering the frame begun by @c BeginFrame.
        virtual void EndFrame() = 0;

        /// @brief Render one frame to this window: @c BeginFrame, draw the layer
        ///        stack, @c EndFrame.
        ///
        /// Convenience wrapper. Callers that need to interleave work with the
        /// live frame (e.g. ImGui rendering) should call @c BeginFrame /
        /// @c EndFrame directly.
        void Draw();

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

        /// @brief Returns the moth_ui layer stack.
        moth_ui::LayerStack& GetLayerStack() const { return *m_layerStack; }

        /// @brief Install the ImGui context that renders into this window.
        ///        Ownership transfers to the window so that ImGui outlives the
        ///        layer stack (whose textures hold descriptor sets allocated
        ///        from ImGui's pool) and is in turn outlived by the surface
        ///        context (which owns that pool).
        ///
        /// ImGui is optional: a Window may run with no ImGui context (for
        /// pure-game windows, or in multi-window setups where only one window
        /// hosts ImGui via its multi-viewport feature). Multi-window setups
        /// with truly independent ImGui state are also supported by giving
        /// each Window its own context — the caller is then responsible for
        /// calling @c ImGui::SetCurrentContext before any ImGui or backend
        /// call belonging to that window.
        void SetImGuiContext(std::unique_ptr<ImGuiContext> imguiContext) { m_imguiContext = std::move(imguiContext); }
        ImGuiContext& GetImGuiContext() const { return *m_imguiContext; }
        bool HasImGuiContext() const { return m_imguiContext != nullptr; }

    protected:
        /// @brief Called after the native window and graphics objects are created.
        void PostCreate();

        /// @brief Release the layer stack and ImGui context in the order they
        ///        depend on each other. Called from a derived class destructor
        ///        body while native handles (window, surface, descriptor pool)
        ///        are still alive — see @c glfw::Window and @c sdl::Window.
        ///        Idempotent.
        void ReleaseUiResources();

        void SetGraphics(std::unique_ptr<graphics::IGraphics> graphics) { m_graphics = std::move(graphics); }
        graphics::IGraphics* GetGraphicsPtr() const { return m_graphics.get(); }

        std::string m_title;
        int m_windowWidth = 0;
        int m_windowHeight = 0;
        IntVec2 m_windowPos = { -1, -1 };
        bool m_windowMaximized = false;

    private:
        // Destruction-order note: subclass member destructors run BEFORE this
        // base destructor's body and members, so we cannot rely on
        // declaration order alone to release the layer stack and ImGui
        // context (which depend on subclass-owned native handles). Subclass
        // destructors call @c ReleaseUiResources() in their body to tear them
        // down while everything is still alive.
        std::unique_ptr<graphics::IGraphics> m_graphics;
        std::unique_ptr<graphics::MothImageFactory> m_mothImageFactory;
        std::unique_ptr<graphics::MothFontFactory> m_mothFontFactory;
        std::unique_ptr<graphics::MothFlipbookFactory> m_mothFlipbookFactory;
        std::unique_ptr<graphics::MothRenderer> m_uiRenderer;
        std::shared_ptr<moth_ui::Context> m_mothContext;
        std::unique_ptr<ImGuiContext> m_imguiContext;
        std::unique_ptr<moth_ui::LayerStack> m_layerStack;
    };
}
