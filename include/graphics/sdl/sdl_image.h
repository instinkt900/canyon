#pragma once

#include "graphics/iimage.h"
#include "graphics/itarget.h"
#include "utils/rect.h"

namespace graphics::sdl {
    class Image : public graphics::IImage, public graphics::ITarget {
    public:
        explicit Image(TextureRef texture);
        Image(TextureRef texture, IntVec2 const& textureDimensions, IntRect const& sourceRect);
        virtual ~Image() = default;

        int GetWidth() const override;
        int GetHeight() const override;
        IntVec2 GetDimensions() const override;
        void ImGui(IntVec2 const& size, FloatVec2 const& uv0, FloatVec2 const& uv1) const override;

        //virtual IntVec2 GetDimensions() const = 0;
        IImage* GetImage() override { return this; }

        TextureRef GetTexture() const {
            return m_texture;
        }

        IntVec2 const& GetTextureDimensions() const {
            return m_textureDimensions;
        }

        IntRect const& GetSourceRect() const {
            return m_sourceRect;
        }

    private:
        TextureRef m_texture;
        IntVec2 m_textureDimensions;
        IntRect m_sourceRect;
    };
}
