#pragma once

#include "canyon/graphics/itexture.h"
#include "canyon/utils/vector.h"
#include "canyon/graphics/sdl/smart_sdl.hpp"

#include <SDL_render.h>

#include <filesystem>
#include <memory>

namespace canyon::graphics::sdl {
    class Texture : public ITexture {
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
