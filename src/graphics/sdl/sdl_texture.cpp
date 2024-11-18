#include "canyon.h"
#include "graphics/sdl/sdl_texture.h"
#include "graphics/sdl/sdl_context.h"
#include "graphics/sdl/smart_sdl.h"

namespace graphics::sdl {
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

    std::unique_ptr<Texture> Texture::FromFile(graphics::sdl::Context const& context, std::filesystem::path const& path) {
        if (auto texture = CreateTextureRef(&context.m_renderer, path)) {
            return std::make_unique<Texture>(texture);
        }
        return nullptr;
    }

}

