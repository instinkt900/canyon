#include "common.h"
#include "canyon/graphics/vulkan/vulkan_asset_context.h"
#include "canyon/graphics/vulkan/vulkan_surface_context.h"
#include "canyon/graphics/vulkan/vulkan_font.h"
#include "canyon/graphics/vulkan/vulkan_image.h"
#include "canyon/graphics/vulkan/vulkan_texture.h"

namespace canyon::graphics::vulkan {
    AssetContext::AssetContext(SurfaceContext& context)
        : m_context(context) {
    }

    std::unique_ptr<IImage> AssetContext::NewImage(std::shared_ptr<ITexture> texture) {
        auto vulkanTexture = std::dynamic_pointer_cast<Texture>(texture);
        if (!vulkanTexture) {
            return nullptr;
        }
        return std::make_unique<Image>(vulkanTexture);
    }

    std::unique_ptr<IImage> AssetContext::NewImage(std::shared_ptr<ITexture> texture, IntRect const& sourceRect) {
        auto vulkanTexture = std::dynamic_pointer_cast<Texture>(texture);
        if (!vulkanTexture) {
            return nullptr;
        }
        return std::make_unique<Image>(vulkanTexture, sourceRect);
    }

    std::unique_ptr<IFont> AssetContext::FontFromFile(std::filesystem::path const& path, uint32_t size) {
        return Font::Load(path, static_cast<int>(size), m_context);
    }

    std::unique_ptr<ITexture> AssetContext::TextureFromFile(std::filesystem::path const& path) {
        return Texture::FromFile(m_context, path);
    }

    std::unique_ptr<IImage> AssetContext::ImageFromFile(std::filesystem::path const& path) {
        auto texture = TextureFromFile(path);
        if (!texture) {
            return nullptr;
        }
        auto* rawTexture = dynamic_cast<Texture*>(texture.get());
        if (rawTexture == nullptr) {
            return nullptr;
        }
        texture.release();
        std::shared_ptr<Texture> vulkanTexture(rawTexture);
        return std::make_unique<Image>(vulkanTexture);
    }
}
