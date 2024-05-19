#pragma once

#include "graphics/itarget.h"
#include "graphics/sdl/sdl_texture.h"
#include "graphics/iimage.h"
#include "utils/rect.h"

namespace graphics::sdl {
    class Image : public graphics::IImage, public graphics::ITarget {
    public:
        explicit Image(std::shared_ptr<Texture> texture);
        Image(std::shared_ptr<Texture> texture, IntRect const& sourceRect);
        virtual ~Image() = default;

        int GetWidth() const override;
        int GetHeight() const override;
        moth_ui::IntVec2 GetDimensions() const override { return { GetWidth(), GetHeight() }; }
        moth_ui::IImage* GetImage() override { return this; }

        void ImGui(moth_ui::IntVec2 const& size, moth_ui::FloatVec2 const& uv0, moth_ui::FloatVec2 const& uv1) const override;

        std::shared_ptr<Texture> GetTexture() const {
            return m_texture;
        }

        IntRect const& GetSourceRect() const {
            return m_sourceRect;
        }

        static std::unique_ptr<Image> Load(SDL_Renderer& renderer, std::filesystem::path const& path);

    private:
        std::shared_ptr<Texture> m_texture;
        IntRect m_sourceRect;
    };
}
