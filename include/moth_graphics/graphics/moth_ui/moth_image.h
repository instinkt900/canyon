#pragma once

#include "moth_graphics/graphics/iimage.h"

#include <moth_ui/utils/vector.h>
#include <moth_ui/graphics/iimage.h>

namespace moth_graphics::graphics {
    /// @brief moth_ui::IImage adapter wrapping a moth_graphics::graphics::Image.
    class MothImage : public moth_ui::IImage {
    public:
        explicit MothImage(Image baseImage);
        ~MothImage() override = default;

        int GetWidth() const override;
        int GetHeight() const override;
        moth_ui::IntVec2 GetDimensions() const override;
        Image const& GetImage() const { return m_baseImage; }

    private:
        Image m_baseImage;
    };
}
