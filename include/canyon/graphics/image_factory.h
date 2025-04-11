#pragma once

#include "canyon/graphics/surface_context.h"
#include "canyon/graphics/iimage.h"
#include "canyon/graphics/itexture.h"
#include "canyon/utils/rect.h"

#include <unordered_map>
#include <string>
#include <memory>
#include <filesystem>

namespace canyon::graphics {
    class ImageFactory {
    public:
        ImageFactory(SurfaceContext& context);
        virtual ~ImageFactory() = default;

        void FlushCache();
        bool LoadTexturePack(std::filesystem::path const& path);

        std::unique_ptr<IImage> GetImage(std::filesystem::path const& path);

    private:
        struct ImageDesc {
            std::shared_ptr<ITexture> m_texture;
            IntRect m_sourceRect;
            std::filesystem::path m_path;
        };

        SurfaceContext& m_context;
        std::unordered_map<std::string, ImageDesc> m_cachedImages;
    };
}
