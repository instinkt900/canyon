#pragma once

#include <SDL_render.h>
#include "graphics/context.h"

namespace graphics::sdl {
    class Context : public graphics::Context {
    public:
        std::unique_ptr<IImage> ImageFromFile(std::filesystem::path const& path) override;
        std::unique_ptr<ITexture> TextureFromFile(std::filesystem::path const& path) override;
        std::unique_ptr<IImage> NewImage(std::shared_ptr<ITexture> texture) override;
        std::unique_ptr<IImage> NewImage(std::shared_ptr<ITexture> texture, IntRect const& sourceRect) override;
        std::unique_ptr<IFont> FontFromFile(std::filesystem::path const& path, int size) override;

        SDL_Renderer* m_renderer = nullptr;
    };
}

