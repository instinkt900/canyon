#include "common.h"
#include "canyon/graphics/sdl/sdl_texture.h"
#include "canyon/graphics/sdl/smart_sdl.hpp"

namespace canyon::graphics::sdl {
    Texture::Texture(SDLTextureRef texture)
        : m_texture(texture) {
        SDL_QueryTexture(texture->GetImpl(), NULL, NULL, &m_textureDimensions.x, &m_textureDimensions.y);
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
        SDL_SetTextureScaleMode(m_texture->GetImpl(), mode);
    }

    std::unique_ptr<Texture> Texture::FromFile(SDL_Renderer* renderer, std::filesystem::path const& path) {
        if (auto texture = CreateTextureRef(renderer, path)) {
            return std::make_unique<Texture>(texture);
        }
        return nullptr;
    }

}

