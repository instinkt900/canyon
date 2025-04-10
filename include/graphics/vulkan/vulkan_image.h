#pragma once

#include "graphics/vulkan/vulkan_texture.h"
#include "graphics/iimage.h"
#include "utils/rect.h"

namespace graphics::vulkan {
    class Image : public graphics::IImage {
    public:
        explicit Image(std::shared_ptr<Texture> texture);
        Image(std::shared_ptr<Texture> texture, IntRect const& sourceRect);
        virtual ~Image();

        int GetWidth() const override;
        int GetHeight() const override;

        static std::unique_ptr<Image> Load(SurfaceContext& context, std::filesystem::path const& path);

        std::shared_ptr<Texture> m_texture;
        IntRect m_sourceRect;

        static class Graphics* s_graphicsContext;
    };
}
