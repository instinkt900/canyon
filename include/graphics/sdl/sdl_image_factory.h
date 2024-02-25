#pragma once

#include "graphics/iimage_factory.h"
#include "graphics/iimage.h"
#include "smart_sdl.h"
#include "utils/rect.h"
#include <unordered_map>

namespace graphics::sdl {
    class ImageFactory : public graphics::IImageFactory {
    public:
        ImageFactory(SDL_Renderer& renderer);
        virtual ~ImageFactory() = default;

        void FlushCache() override;
        bool LoadTexturePack(std::filesystem::path const& path) override;

        std::unique_ptr<graphics::IImage> GetImage(std::filesystem::path const& path) override;

    private:
        SDL_Renderer& m_renderer;

        struct ImageDesc {
            TextureRef m_texture;
            IntRect m_sourceRect;
            std::string m_path;
        };

        std::unordered_map<std::string, ImageDesc> m_cachedImages;
    };
}
