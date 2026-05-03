#pragma once

#include "moth_graphics/graphics/itarget.h"
#include "moth_graphics/graphics/sdl/sdl_texture.h"

#include <memory>

namespace moth_graphics::graphics::sdl {
    /// @brief SDL render target. Backed by a single texture which can also be
    ///        sampled as an image (via @c GetImage()).
    class Target : public ITarget {
    public:
        explicit Target(std::shared_ptr<Texture> texture);
        ~Target() override = default;

        Image GetImage() const override;

        std::shared_ptr<Texture> const& GetSDLTexture() const { return m_texture; }

    private:
        std::shared_ptr<Texture> m_texture;
    };
}
