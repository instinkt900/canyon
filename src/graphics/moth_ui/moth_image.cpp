#include "graphics/moth_ui/moth_image.h"
#include "graphics/iimage.h"

namespace graphics {
    MothImage::MothImage(std::shared_ptr<graphics::IImage> baseImage)
        : m_baseImage(baseImage) {
    }

    int MothImage::GetWidth() const {
        return m_baseImage->GetWidth();
    }

    int MothImage::GetHeight() const {
        return m_baseImage->GetHeight();
    }

    moth_ui::IntVec2 MothImage::GetDimensions() const {
        return { m_baseImage->GetWidth(), m_baseImage->GetHeight() };
    }

    void MothImage::ImGui(moth_ui::IntVec2 const& size, moth_ui::FloatVec2 const& uv0, moth_ui::FloatVec2 const& uv1) const {
    }
}
