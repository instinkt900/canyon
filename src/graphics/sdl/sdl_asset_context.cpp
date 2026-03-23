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

    std::unique_ptr<ITexture> AssetContext::TextureFromPixels(int width, int height, uint8_t const* pixels) {
        // Use a streaming texture so we can lock and write the pixel data directly.
        // SDL_PIXELFORMAT_ABGR8888 stores bytes as R,G,B,A in memory on little-endian
        // systems, matching our input layout.
        SDL_Texture* tex = SDL_CreateTexture(m_context.GetRenderer(), SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, width, height);
        if (tex == nullptr) {
            return nullptr;
        }
        void* texData = nullptr;
        int pitch = 0;
        if (SDL_LockTexture(tex, nullptr, &texData, &pitch) == 0) {
            int const rowBytes = width * 4;
            for (int row = 0; row < height; ++row) {
                auto* dst = static_cast<uint8_t*>(texData) + (row * pitch);
                auto const* src = pixels + (row * rowBytes);
                std::copy(src, src + rowBytes, dst);
            }
            SDL_UnlockTexture(tex);
        } else {
            spdlog::error("TextureFromPixels: SDL_LockTexture failed: {}", SDL_GetError());
        }
        return std::make_unique<Texture>(CreateTextureRef(tex));
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
