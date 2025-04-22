#pragma once

#include "canyon/graphics/itarget.h"
#include "canyon/graphics/sdl/sdl_surface_context.h"
#include "canyon/graphics/sdl/sdl_texture.h"
#include "canyon/utils/rect.h"

#include <memory>
#include <filesystem>

namespace canyon::graphics::sdl {
    class Image : public ITarget, public IImage {
    public:
        explicit Image(std::shared_ptr<Texture> texture);
        Image(std::shared_ptr<Texture> texture, IntRect const& sourceRect);
        virtual ~Image() = default;

        int GetWidth() const override;
        int GetHeight() const override;
        IImage* GetImage() override { return this; }

        void ImGui(canyon::IntVec2 const& size, canyon::FloatVec2 const& uv0 = { 0, 0 }, canyon::FloatVec2 const& uv1 = { 1, 1 }) const override;

        std::shared_ptr<Texture> GetTexture() const {
            return m_texture;
        }

        IntRect const& GetSourceRect() const {
            return m_sourceRect;
        }

        static std::unique_ptr<Image> Load(SurfaceContext& context, std::filesystem::path const& path);

    private:
        std::shared_ptr<Texture> m_texture;
        IntRect m_sourceRect;
    };
}
