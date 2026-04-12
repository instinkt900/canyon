#include "common.h"
#include "moth_graphics/graphics/moth_ui/moth_image.h"
#include "moth_graphics/graphics/iimage.h"

namespace moth_graphics::graphics {
    MothImage::MothImage(std::shared_ptr<moth_graphics::graphics::IImage> baseImage)
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

}
