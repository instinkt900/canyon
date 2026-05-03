#include "common.h"
#include "moth_graphics/graphics/vulkan/vulkan_asset_context.h"
#include "moth_graphics/graphics/vulkan/vulkan_surface_context.h"
#include "moth_graphics/graphics/vulkan/vulkan_font.h"
#include "moth_graphics/graphics/vulkan/vulkan_texture.h"

namespace moth_graphics::graphics::vulkan {
    AssetContext::AssetContext(SurfaceContext& context)
        : m_context(context)
        , m_textureFactory(*this)
        , m_fontFactory(*this)
        , m_spriteSheetFactory(*this) {
    }

    std::unique_ptr<IFont> AssetContext::FontFromFile(std::filesystem::path const& path, uint32_t size) {
        return Font::Load(path, static_cast<int>(size), m_context);
    }

    std::unique_ptr<ITexture> AssetContext::TextureFromFile(std::filesystem::path const& path) {
        return Texture::FromFile(m_context, path);
    }

    std::unique_ptr<ITexture> AssetContext::TextureFromPixels(int width, int height, uint8_t const* pixels) {
        return Texture::FromRGBA(m_context, width, height, pixels);
    }
}
