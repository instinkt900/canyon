#pragma once

#include "graphics/itexture.h"
#include "utils/vector.h"
#include "graphics/sdl/smart_sdl.h"

namespace graphics::sdl {
    class Texture : public graphics::ITexture {
    public:
        explicit Texture(SDLTextureRef texture);
        virtual ~Texture() = default;

        int GetWidth() const override;
        int GetHeight() const override;

        SDLTextureRef GetSDLTexture() const { return m_texture; }

        static std::unique_ptr<Texture> FromFile(SDL_Renderer* renderer, std::filesystem::path const& path);

    private:
        SDLTextureRef m_texture;
        IntVec2 m_textureDimensions;
    };
}

