#pragma once

#include "canyon/graphics/vulkan/vulkan_texture.h"
#include "canyon/graphics/vulkan/vulkan_surface_context.h"
#include "canyon/graphics/iimage.h"
#include "canyon/utils/rect.h"

#include <memory>
#include <filesystem>

namespace canyon::graphics::vulkan {
    class Image : public IImage {
    public:
        explicit Image(std::shared_ptr<Texture> texture);
        Image(std::shared_ptr<Texture> texture, IntRect const& sourceRect);
        virtual ~Image();

        int GetWidth() const override;
        int GetHeight() const override;

        void ImGui(canyon::IntVec2 const& size, canyon::FloatVec2 const& uv0 = { 0, 0 }, canyon::FloatVec2 const& uv1 = { 1, 1 }) const override;

        static std::unique_ptr<Image> Load(SurfaceContext& context, std::filesystem::path const& path);

        std::shared_ptr<Texture> m_texture;
        IntRect m_sourceRect;
    };
}
