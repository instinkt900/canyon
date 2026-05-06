#include "common.h"
#include "sdl_asset_context.h"
#include "moth_graphics/graphics/sdl/sdl_surface_context.h"
#include "sdl_font.h"
#include "sdl_texture.h"

namespace moth_graphics::graphics::sdl {
    AssetContext::AssetContext(SurfaceContext& context)
        : m_context(context)
        , m_textureFactory(*this)
        , m_fontFactory(*this)
        , m_spriteSheetFactory(*this) {
    }

    std::unique_ptr<IFont> AssetContext::FontFromFile(std::filesystem::path const& path, uint32_t size) {
        return Font::Load(*m_context.GetRenderer(), path, static_cast<int>(size));
    }

    std::unique_ptr<ITexture> AssetContext::TextureFromFile(std::filesystem::path const& path) {
        return Texture::FromFile(m_context.GetRenderer(), path);
    }

    std::unique_ptr<ITexture> AssetContext::TextureFromPixels(int width, int height, uint8_t const* pixels) {
        if (pixels == nullptr) {
            return nullptr;
        }
        // Use a streaming texture so we can lock and write the pixel data directly.
        // SDL_PIXELFORMAT_RGBA32 is an endian-neutral alias that always matches
        // pixels laid out in memory as R, G, B, A bytes.
        SDL_Texture* tex = SDL_CreateTexture(m_context.GetRenderer(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, width, height);
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
            SDL_DestroyTexture(tex);
            return nullptr;
        }
        return std::make_unique<Texture>(m_context.GetRenderer(), CreateTextureRef(tex));
    }
}
