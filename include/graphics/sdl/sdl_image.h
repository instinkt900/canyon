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
        moth_ui::IntVec2 GetDimensions() const override;
        void ImGui(moth_ui::IntVec2 const& size, moth_ui::FloatVec2 const& uv0, moth_ui::FloatVec2 const& uv1) const override;

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

        static std::unique_ptr<Image> Load(SDL_Renderer& renderer, std::filesystem::path const& path);

    private:
        TextureRef m_texture;
        IntVec2 m_textureDimensions;
        IntRect m_sourceRect;
    };
}
