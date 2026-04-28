#include "common.h"
#include "moth_graphics/graphics/spritesheet.h"

namespace moth_graphics::graphics {
    SpriteSheet::SpriteSheet(std::shared_ptr<IImage> image,
                             std::vector<FrameEntry> frames,
                             std::vector<ClipEntry> clips)
        : m_image(std::move(image))
        , m_frames(std::move(frames))
        , clips(std::move(clips)) {
    }

    std::shared_ptr<IImage> SpriteSheet::GetImage() const {
        return m_image;
    }

    int SpriteSheet::GetFrameCount() const {
        return static_cast<int>(m_frames.size());
    }

    bool SpriteSheet::GetFrameDesc(int index, FrameEntry& outEntry) const {
        if (index < 0 || index >= static_cast<int>(m_frames.size())) {
            return false;
        }
        outEntry = m_frames[static_cast<size_t>(index)];
        return true;
    }

    int SpriteSheet::GetClipCount() const {
        return static_cast<int>(clips.size());
    }

    std::string_view SpriteSheet::GetClipName(int index) const {
        if (index < 0 || index >= static_cast<int>(clips.size())) {
            return {};
        }
        return clips[static_cast<size_t>(index)].name;
    }

    bool SpriteSheet::GetClipDesc(std::string_view name, ClipDesc& outDesc) const {
        for (auto const& entry : clips) {
            if (entry.name == name) {
                outDesc = entry.desc;
                return true;
            }
        }
        return false;
    }
}
