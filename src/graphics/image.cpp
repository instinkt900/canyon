#include "common.h"
#include "moth_graphics/graphics/iimage.h"

namespace moth_graphics::graphics {
    Image::Image(std::shared_ptr<ITexture> texture)
        : m_texture(std::move(texture)) {
        if (m_texture) {
            m_sourceRect = IntRect{ { 0, 0 }, { m_texture->GetWidth(), m_texture->GetHeight() } };
        }
    }

    Image::Image(std::shared_ptr<ITexture> texture, IntRect const& sourceRect)
        : m_texture(std::move(texture))
        , m_sourceRect(sourceRect) {
    }

    void Image::DrawImGui(IntVec2 const& size) const {
        DrawImGui(size, { 0, 0 }, { 1, 1 });
    }

    void Image::DrawImGui(IntVec2 const& size, FloatVec2 const& uv0, FloatVec2 const& uv1) const {
        if (m_texture) {
            float const texW = static_cast<float>(m_texture->GetWidth());
            float const texH = static_cast<float>(m_texture->GetHeight());
            float const srcW = static_cast<float>(m_sourceRect.w());
            float const srcH = static_cast<float>(m_sourceRect.h());
            float const offX = static_cast<float>(m_sourceRect.topLeft.x);
            float const offY = static_cast<float>(m_sourceRect.topLeft.y);
            FloatVec2 const remappedUv0{
                (offX + uv0.x * srcW) / texW,
                (offY + uv0.y * srcH) / texH };
            FloatVec2 const remappedUv1{
                (offX + uv1.x * srcW) / texW,
                (offY + uv1.y * srcH) / texH };
            m_texture->DrawImGui(size, remappedUv0, remappedUv1);
        }
    }

    void Image::SaveToPNG(std::filesystem::path const& path) {
        if (m_texture) {
            m_texture->SaveToPNG(path, m_sourceRect);
        }
    }
}
