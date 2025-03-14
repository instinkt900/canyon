#pragma once

#include "graphics/iimage.h"
#include "moth_ui/utils/vector.h"
#include "moth_ui/iimage.h"

namespace graphics {
    class MothImage : public moth_ui::IImage {
    public:
        MothImage(std::shared_ptr<graphics::IImage> baseImage);
        virtual ~MothImage() = default;

        int GetWidth() const override;
        int GetHeight() const override;
        moth_ui::IntVec2 GetDimensions() const override;
        std::shared_ptr<graphics::IImage> GetImage() { return m_baseImage; }

        void ImGui(moth_ui::IntVec2 const& size, moth_ui::FloatVec2 const& uv0 = { 0, 0 }, moth_ui::FloatVec2 const& uv1 = { 1, 1 }) const override;

    private:
        std::shared_ptr<graphics::IImage> m_baseImage;
    };
}
