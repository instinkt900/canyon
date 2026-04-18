#include "common.h"
#include "moth_graphics/graphics/sdl/sdl_texture.h"
#include "moth_graphics/graphics/sdl/smart_sdl.hpp"

namespace moth_graphics::graphics::sdl {
    Texture::Texture(SDL_Renderer* renderer, SDLTextureRef texture)
        : m_renderer(renderer)
        , m_texture(texture) {
        SDL_QueryTexture(texture->GetImpl(), NULL, NULL, &m_textureDimensions.x, &m_textureDimensions.y);
        Texture::SetFilter(TextureFilter::Linear, TextureFilter::Linear);
    }

    int Texture::GetWidth() const {
        return m_textureDimensions.x;
    }

    int Texture::GetHeight() const {
        return m_textureDimensions.y;
    }

    void Texture::SetFilter(TextureFilter minFilter, TextureFilter magFilter) {
        // SDL2 has a single scale mode rather than separate min/mag filters.
        // Use Nearest if either filter requests it, Linear if both agree.
        SDL_ScaleMode const mode = (minFilter == TextureFilter::Nearest || magFilter == TextureFilter::Nearest)
                                       ? SDL_ScaleModeNearest
                                       : SDL_ScaleModeLinear;

        // SDL2's hardware renderer batches draw calls and applies per-texture state
        // (scale mode) at flush time, not at enqueue time. If two nodes share the same
        // SDL_Texture but need different scale modes, the last SetFilter call would win
        // for both. Flushing before a mode change forces any pending batched draws to
        // commit at the old mode before we switch it.
        SDL_ScaleMode currentMode = SDL_ScaleModeLinear;
        if (SDL_GetTextureScaleMode(m_texture->GetImpl(), &currentMode) == 0 && currentMode != mode) {
            SDL_RenderFlush(m_renderer);
        }

        SDL_SetTextureScaleMode(m_texture->GetImpl(), mode);
    }

    std::unique_ptr<Texture> Texture::FromFile(SDL_Renderer* renderer, std::filesystem::path const& path) {
        auto surface = CreateSurfaceRef(path);
        if (!surface) {
            return nullptr;
        }
        auto texture = CreateTextureRef(renderer, surface);
        if (!texture || texture->GetImpl() == nullptr) {
            return nullptr;
        }
        return std::make_unique<Texture>(renderer, texture);
    }

}

