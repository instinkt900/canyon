#pragma once

#include "moth_graphics/graphics/itarget.h"
#include "moth_graphics/graphics/sdl/sdl_surface_context.h"
#include "moth_graphics/graphics/sdl/sdl_texture.h"
#include "moth_graphics/utils/rect.h"

#include <memory>
#include <filesystem>

namespace moth_graphics::graphics::sdl {
    class Image : public ITarget, public IImage {
    public:
        explicit Image(std::shared_ptr<Texture> texture);
        Image(std::shared_ptr<Texture> texture, IntRect const& sourceRect);
        ~Image() override = default;

        int GetWidth() const override;
        int GetHeight() const override;
        std::shared_ptr<ITexture> GetTexture() const override;
        IImage* GetImage() override { return this; }

        void ImGui(moth_graphics::IntVec2 const& size, moth_graphics::FloatVec2 const& uv0 = { 0, 0 }, moth_graphics::FloatVec2 const& uv1 = { 1, 1 }) const override;

        IntRect const& GetSourceRect() const {
            return m_sourceRect;
        }

        static std::unique_ptr<Image> Load(SurfaceContext& context, std::filesystem::path const& path);

    private:
        std::shared_ptr<Texture> m_texture;
        IntRect m_sourceRect;
    };
}
