#pragma once

#include "moth_graphics/graphics/vulkan/vulkan_texture.h"
#include "moth_graphics/graphics/vulkan/vulkan_surface_context.h"
#include "moth_graphics/graphics/iimage.h"
#include "moth_graphics/utils/rect.h"

#include <memory>
#include <filesystem>

namespace moth_graphics::graphics::vulkan {
    class Image : public IImage {
    public:
        explicit Image(std::shared_ptr<Texture> texture);
        Image(std::shared_ptr<Texture> texture, IntRect const& sourceRect);
        ~Image() override;

        int GetWidth() const override;
        int GetHeight() const override;
        std::shared_ptr<ITexture> GetTexture() const override;

        void ImGui(moth_graphics::IntVec2 const& size, moth_graphics::FloatVec2 const& uv0 = { 0, 0 }, moth_graphics::FloatVec2 const& uv1 = { 1, 1 }) const override;

        static std::unique_ptr<Image> Load(SurfaceContext& context, std::filesystem::path const& path);

        std::shared_ptr<Texture> const& GetVkTexture() const { return m_texture; }
        IntRect const& GetSourceRect() const { return m_sourceRect; }

    private:
        std::shared_ptr<Texture> m_texture;
        IntRect m_sourceRect;
    };
}
