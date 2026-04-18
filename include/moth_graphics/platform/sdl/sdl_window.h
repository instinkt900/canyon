#pragma once

#include "moth_graphics/events/event_window.h"
#include "moth_graphics/graphics/sdl/sdl_context.h"
#include "moth_graphics/graphics/sdl/sdl_surface_context.h"
#include "moth_graphics/graphics/surface_context.h"
#include "moth_graphics/platform/window.h"

#include <SDL_video.h>
#include <SDL_render.h>

#include <memory>
#include <string_view>
#include <cstdint>

namespace moth_graphics::platform::sdl {
    class Window : public platform::Window {
    public:
        Window(graphics::sdl::Context& context, std::string_view applicationTitle, int width, int height);
        ~Window() override;

        graphics::SurfaceContext & GetSurfaceContext() const override { return *m_surfaceContext; }
        void SetWindowTitle(std::string_view title) override;

        SDL_Window* GetSDLWindow() const { return m_window; }
        SDL_Renderer* GetSDLRenderer() const { return m_renderer; }

        void Update(uint32_t ticks) override;
        void Draw() override;

    private:
        bool CreateWindow();
        void DestroyWindow();


        graphics::sdl::Context& m_context;
        std::unique_ptr<graphics::sdl::SurfaceContext> m_surfaceContext;

        uint32_t m_windowId = 0;
        SDL_Window* m_window = nullptr;
        SDL_Renderer* m_renderer = nullptr;

        bool OnResizeEvent(EventWindowSize const& event);
    };
}

