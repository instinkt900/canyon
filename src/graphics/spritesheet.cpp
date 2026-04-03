#include "common.h"
#include "moth_graphics/graphics/spritesheet.h"

namespace moth_graphics::graphics {
    SpriteSheet::SpriteSheet(std::shared_ptr<IImage> image,
                             SheetDesc sheetDesc,
                             std::vector<ClipEntry> clips)
        : m_image(std::move(image))
        , m_sheetDesc(sheetDesc)
        , m_clips(std::move(clips)) {
        m_sheetDesc.NumClips = static_cast<int>(m_clips.size());
    }

    std::shared_ptr<IImage> SpriteSheet::GetImage() const {
        return m_image;
    }

    void SpriteSheet::GetSheetDesc(SheetDesc& outDesc) const {
        outDesc = m_sheetDesc;
    }

    std::string_view SpriteSheet::GetClipName(int index) const {
        if (index < 0 || index >= static_cast<int>(m_clips.size())) {
            return {};
        }
        return m_clips[static_cast<size_t>(index)].name;
    }

    bool SpriteSheet::GetClipDesc(std::string_view name, ClipDesc& outDesc) const {
        for (auto const& entry : m_clips) {
            if (entry.name == name) {
                outDesc = entry.desc;
                return true;
            }
        }
        return false;
    }
}
