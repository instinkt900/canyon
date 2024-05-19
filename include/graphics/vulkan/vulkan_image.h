#pragma once

#include "graphics/vulkan/vulkan_texture.h"
#include "graphics/iimage.h"
#include "utils/rect.h"

namespace graphics::vulkan {
    class Image : public graphics::IImage {
    public:
        Image(std::shared_ptr<Texture> texture, IntRect const& sourceRect);
        virtual ~Image();

        int GetWidth() const override;
        int GetHeight() const override;
        moth_ui::IntVec2 GetDimensions() const override;
        void ImGui(moth_ui::IntVec2 const& size, moth_ui::FloatVec2 const& uv0, moth_ui::FloatVec2 const& uv1) const override;
        static std::unique_ptr<Image> Load(Context& context, std::filesystem::path const& path);

        std::shared_ptr<Texture> m_texture;
        IntRect m_sourceRect;

        static class Graphics* s_graphicsContext;
    };
}
