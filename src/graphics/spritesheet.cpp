#include "common.h"
#include "moth_graphics/graphics/spritesheet.h"

#include <optional>

namespace moth_graphics::graphics {
    SpriteSheet::SpriteSheet(Image image,
                             std::vector<FrameEntry> frames,
                             std::vector<ClipEntry> clips)
        : m_image(std::move(image))
        , m_frames(std::move(frames))
        , m_clips(std::move(clips)) {
    }

    Image const& SpriteSheet::GetImage() const {
        return m_image;
    }

    int SpriteSheet::GetFrameCount() const {
        return static_cast<int>(m_frames.size());
    }

    std::optional<SpriteSheet::FrameEntry> SpriteSheet::GetFrameDesc(int index) const {
        if (index < 0 || index >= static_cast<int>(m_frames.size())) {
            return std::nullopt;
        }
        return m_frames[static_cast<size_t>(index)];
    }

    int SpriteSheet::GetClipCount() const {
        return static_cast<int>(m_clips.size());
    }

    std::string_view SpriteSheet::GetClipName(int index) const {
        if (index < 0 || index >= static_cast<int>(m_clips.size())) {
            return {};
        }
        return m_clips[static_cast<size_t>(index)].name;
    }

    std::optional<SpriteSheet::ClipDesc> SpriteSheet::GetClipDesc(std::string_view name) const {
        for (auto const& entry : m_clips) {
            if (entry.name == name) {
                return entry.desc;
            }
        }
        return std::nullopt;
    }
}
