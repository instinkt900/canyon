#pragma once

#include "graphics/iimage.h"
#include "utils/rect.h"
#include "vulkan_image.h"

namespace graphics::vulkan {
    class SubImage : public graphics::IImage {
    public:
        SubImage(std::shared_ptr<Image> texture, IntVec2 const& textureDimensions, IntRect const& sourceRect);
        virtual ~SubImage();

        int GetWidth() const override;
        int GetHeight() const override;
        moth_ui::IntVec2 GetDimensions() const override;
        void ImGui(moth_ui::IntVec2 const& size, moth_ui::FloatVec2 const& uv0, moth_ui::FloatVec2 const& uv1) const override;

        std::shared_ptr<Image> m_texture;
        IntVec2 m_textureDimensions;
        IntRect m_sourceRect;

        static class Graphics* s_graphicsContext;
    };
}
