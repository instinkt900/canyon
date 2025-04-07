#pragma once

#include "canyon/events/event_window.h"
#include "canyon/graphics/sdl/sdl_context.h"
#include "canyon/graphics/sdl/sdl_surface_context.h"
#include "canyon/graphics/surface_context.h"
#include "canyon/platform/window.h"

#include <SDL_video.h>
#include <SDL_render.h>

#include <memory>
#include <string>
#include <cstdint>

namespace platform::sdl {
    class Window : public platform::Window {
    public:
        Window(graphics::sdl::Context& context, std::string const& applicationTitle, int width, int height);
        virtual ~Window();

        graphics::SurfaceContext & GetSurfaceContext() const override { return *m_surfaceContext; }
        void SetWindowTitle(std::string const& title) override;

        SDL_Window* GetSDLWindow() const { return m_window; }
        SDL_Renderer* GetSDLRenderer() const { return m_renderer; }

        void Update(uint32_t ticks) override;
        void Draw() override;

    protected:
        bool CreateWindow() override;
        void DestroyWindow() override;

    private:
        graphics::sdl::Context& m_context;
        std::unique_ptr<graphics::sdl::SurfaceContext> m_surfaceContext;

        uint32_t m_windowId = 0;
        SDL_Window* m_window = nullptr;
        SDL_Renderer* m_renderer = nullptr;

        bool OnResizeEvent(EventWindowSize const& event);
    };
}

