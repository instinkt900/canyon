#pragma once

#include "sdl_asset_context.h"
#include "moth_graphics/graphics/surface_context.h"

#include <SDL_render.h>

namespace moth_graphics::graphics::sdl {
    class SurfaceContext : public graphics::SurfaceContext {
    public:
        SurfaceContext(SDL_Renderer* renderer);
        ~SurfaceContext() override = default;

        graphics::AssetContext& GetAssetContext() override { return m_assetContext; }

        // Extension methods — not on the base SurfaceContext interface.
        SDL_Renderer* GetRenderer() const { return m_renderer; }

    private:
        SDL_Renderer* m_renderer = nullptr;
        AssetContext m_assetContext;
    };
}
