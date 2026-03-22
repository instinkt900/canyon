#pragma once

#include "moth_graphics/graphics/sdl/sdl_asset_context.h"
#include "moth_graphics/graphics/sdl/sdl_context.h"
#include "moth_graphics/graphics/surface_context.h"

#include <SDL_render.h>

namespace moth_graphics::graphics::sdl {
    class SurfaceContext : public graphics::SurfaceContext {
    public:
        SurfaceContext(Context& context, SDL_Renderer* renderer);
        ~SurfaceContext() override = default;

        graphics::AssetContext& GetAssetContext() override { return m_assetContext; }

        // Internal — not part of the public SurfaceContext interface.
        Context& GetContext() const { return m_context; }
        SDL_Renderer* GetRenderer() const { return m_renderer; }

    private:
        Context& m_context;
        SDL_Renderer* m_renderer = nullptr;
        AssetContext m_assetContext;
    };
}
