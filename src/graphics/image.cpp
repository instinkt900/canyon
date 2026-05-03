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

    void Image::DrawImGui(IntVec2 const& size, FloatVec2 const& uv0, FloatVec2 const& uv1) const {
        if (m_texture) {
            m_texture->DrawImGui(size, uv0, uv1);
        }
    }

    void Image::SaveToPNG(std::filesystem::path const& path) {
        if (m_texture) {
            m_texture->SaveToPNG(path, m_sourceRect);
        }
    }
}
