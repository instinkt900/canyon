#pragma once

#include "graphics/surface_context.h"
#include "graphics/sdl/sdl_context.h"
#include <SDL_render.h>

namespace graphics::sdl {
    class SurfaceContext : public graphics::SurfaceContext {
    public:
        SurfaceContext(Context& context, SDL_Renderer* renderer);
        virtual ~SurfaceContext() = default;

        Context& GetContext() const override { return m_context; }
        SDL_Renderer* GetRenderer() const { return m_renderer; }

        std::unique_ptr<IImage> NewImage(std::shared_ptr<ITexture> texture) override;
        std::unique_ptr<IImage> NewImage(std::shared_ptr<ITexture> texture, IntRect const& sourceRect) override;
        std::unique_ptr<IFont> FontFromFile(std::filesystem::path const& path, uint32_t size) override;
        std::unique_ptr<ITexture> TextureFromFile(std::filesystem::path const& path) override;

    private:
        Context& m_context;
        SDL_Renderer* m_renderer = nullptr;
    };
}

