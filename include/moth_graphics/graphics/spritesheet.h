#pragma once

#include "moth_graphics/graphics/ispritesheet.h"

#include <string>
#include <vector>

namespace moth_graphics::graphics {
    /// @brief Concrete sprite sheet loaded from a .flipbook.json descriptor.
    class SpriteSheet : public ISpriteSheet {
    public:
        struct ClipEntry {
            std::string name;
            ClipDesc desc;
        };

        SpriteSheet(std::shared_ptr<IImage> image,
                    SheetDesc sheetDesc,
                    std::vector<ClipEntry> clips);
        ~SpriteSheet() override = default;

        std::shared_ptr<IImage> GetImage() const override;
        void GetSheetDesc(SheetDesc& outDesc) const override;
        std::string_view GetClipName(int index) const override;
        bool GetClipDesc(std::string_view name, ClipDesc& outDesc) const override;

    private:
        std::shared_ptr<IImage> m_image;
        SheetDesc m_sheetDesc;
        std::vector<ClipEntry> m_clips;
    };
}
