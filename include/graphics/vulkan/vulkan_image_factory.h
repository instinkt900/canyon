#pragma once

#include "graphics/iimage_factory.h"
#include "graphics/iimage.h"
#include "vulkan_context.h"
#include "vulkan_image.h"
#include "vulkan_graphics.h"

namespace graphics::vulkan {
    class ImageFactory : public IImageFactory {
    public:
        ImageFactory(Context& m_context, Graphics& graphics);
        virtual ~ImageFactory() = default;

        void FlushCache() override;
        bool LoadTexturePack(std::filesystem::path const& path) override;

        std::unique_ptr<IImage> GetImage(std::filesystem::path const& path) override;

    private:
        Context& m_context;

        struct ImageDesc {
            std::shared_ptr<Image> m_texture;
            IntRect m_sourceRect;
            std::string m_path;
        };

        std::unordered_map<std::string, ImageDesc> m_cachedImages;
    };
}
