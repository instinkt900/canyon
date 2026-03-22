#include "common.h"
#include "moth_graphics/graphics/sdl/sdl_asset_context.h"
#include "moth_graphics/graphics/sdl/sdl_surface_context.h"
#include "moth_graphics/graphics/sdl/sdl_font.h"
#include "moth_graphics/graphics/sdl/sdl_image.h"
#include "moth_graphics/graphics/sdl/sdl_texture.h"

namespace moth_graphics::graphics::sdl {
    AssetContext::AssetContext(SurfaceContext& context)
        : m_context(context) {
    }

    std::unique_ptr<IImage> AssetContext::NewImage(std::shared_ptr<ITexture> texture) {
        auto sdlTexture = std::dynamic_pointer_cast<Texture>(texture);
        if (!sdlTexture) {
            return nullptr;
        }
        return std::make_unique<Image>(sdlTexture);
    }

    std::unique_ptr<IImage> AssetContext::NewImage(std::shared_ptr<ITexture> texture, IntRect const& sourceRect) {
        auto sdlTexture = std::dynamic_pointer_cast<Texture>(texture);
        if (!sdlTexture) {
            return nullptr;
        }
        return std::make_unique<Image>(sdlTexture, sourceRect);
    }

    std::unique_ptr<IFont> AssetContext::FontFromFile(std::filesystem::path const& path, uint32_t size) {
        return Font::Load(*m_context.GetRenderer(), path, static_cast<int>(size));
    }

    std::unique_ptr<ITexture> AssetContext::TextureFromFile(std::filesystem::path const& path) {
        return Texture::FromFile(m_context.GetRenderer(), path);
    }

    std::unique_ptr<IImage> AssetContext::ImageFromFile(std::filesystem::path const& path) {
        auto rawTexture = TextureFromFile(path);
        if (!rawTexture) {
            return nullptr;
        }
        std::shared_ptr<Texture> texture(dynamic_cast<Texture*>(rawTexture.release()));
        if (!texture) {
            return nullptr;
        }
        return std::make_unique<Image>(texture);
    }
}
