#pragma once

#include "moth_graphics/graphics/itexture.h"
#include "moth_graphics/utils/vector.h"
#include "moth_graphics/graphics/sdl/smart_sdl.hpp"

#include <SDL_render.h>

#include <filesystem>
#include <memory>

namespace moth_graphics::graphics::sdl {
    class Texture : public ITexture {
    public:
        Texture(SDL_Renderer* renderer, SDLTextureRef texture);
        ~Texture() override = default;

        int GetWidth() const override;
        int GetHeight() const override;
        void SetFilter(TextureFilter minFilter, TextureFilter magFilter) override;
        void SetAddressMode(TextureAddressMode u, TextureAddressMode v) override {} // not supported in SDL2

        SDLTextureRef GetSDLTexture() const { return m_texture; }

        static std::unique_ptr<Texture> FromFile(SDL_Renderer* renderer, std::filesystem::path const& path);

    private:
        SDL_Renderer* m_renderer;
        SDLTextureRef m_texture;
        IntVec2 m_textureDimensions;
    };
}
