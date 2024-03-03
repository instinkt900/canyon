#include "graphics/vulkan/vulkan_subimage.h"
#include "graphics/vulkan/vulkan_graphics.h"
#include <imgui.h>

namespace graphics::vulkan {
    Graphics* SubImage::s_graphicsContext = nullptr;

    SubImage::SubImage(std::shared_ptr<Image> texture, IntVec2 const& textureDimensions, IntRect const& sourceRect)
        : m_texture(texture)
        , m_textureDimensions(textureDimensions)
        , m_sourceRect(sourceRect) {
    }

    SubImage::~SubImage() {
    }

    int SubImage::GetWidth() const {
        return m_sourceRect.bottomRight.x - m_sourceRect.topLeft.x;
    }

    int SubImage::GetHeight() const {
        return m_sourceRect.bottomRight.y - m_sourceRect.topLeft.y;
    }

    IntVec2 SubImage::GetDimensions() const {
        return { GetWidth(), GetHeight() };
    }

    void SubImage::ImGui(IntVec2 const& size, FloatVec2 const& uv0, FloatVec2 const& uv1) const {
        if (m_texture && s_graphicsContext) {
            ImGui::Image(m_texture->GetDescriptorSet(),
                            ImVec2(static_cast<float>(size.x), static_cast<float>(size.y)),
                            ImVec2(uv0.x, uv0.y),
                            ImVec2(uv1.x, uv1.y));
        }
    }
}
