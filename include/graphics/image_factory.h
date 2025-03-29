#pragma once

#include "graphics/surface_context.h"
#include "graphics/iimage.h"
#include "graphics/itexture.h"
#include "utils/rect.h"

namespace graphics {
    class ImageFactory {
    public:
        ImageFactory(graphics::SurfaceContext& context);
        virtual ~ImageFactory() = default;

        void FlushCache();
        bool LoadTexturePack(std::filesystem::path const& path);

        std::unique_ptr<IImage> GetImage(std::filesystem::path const& path);

    private:
        struct ImageDesc {
            std::shared_ptr<ITexture> m_texture;
            IntRect m_sourceRect;
            std::string m_path;
        };

        graphics::SurfaceContext& m_context;
        std::unordered_map<std::string, ImageDesc> m_cachedImages;
    };
}

