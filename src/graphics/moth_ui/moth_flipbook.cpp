#include "common.h"
#include "moth_graphics/graphics/moth_ui/moth_flipbook.h"

namespace moth_graphics::graphics {
    namespace {
        moth_ui::IFlipbook::LoopType ToMothLoopType(graphics::SpriteSheet::LoopType t) {
            switch (t) {
            case graphics::SpriteSheet::LoopType::Stop:  return moth_ui::IFlipbook::LoopType::Stop;
            case graphics::SpriteSheet::LoopType::Reset: return moth_ui::IFlipbook::LoopType::Reset;
            case graphics::SpriteSheet::LoopType::Loop:  return moth_ui::IFlipbook::LoopType::Loop;
            default:                                      return moth_ui::IFlipbook::LoopType::Stop;
            }
        }
    }

    MothFlipbook::MothFlipbook(std::shared_ptr<graphics::SpriteSheet> spriteSheet)
        : m_spriteSheet(std::move(spriteSheet))
        , m_image(m_spriteSheet->GetImage()) {
    }

    moth_ui::IImage const& MothFlipbook::GetImage() const {
        return m_image;
    }

    int MothFlipbook::GetFrameCount() const {
        return m_spriteSheet->GetFrameCount();
    }

    bool MothFlipbook::GetFrameDesc(int index, moth_ui::IFlipbook::FrameDesc& outDesc) const {
        graphics::SpriteSheet::FrameEntry entry;
        if (!m_spriteSheet->GetFrameDesc(index, entry)) {
            return false;
        }
        outDesc.rect  = entry.rect;
        outDesc.pivot = entry.pivot;
        return true;
    }

    int MothFlipbook::GetClipCount() const {
        return m_spriteSheet->GetClipCount();
    }

    std::string_view MothFlipbook::GetClipName(int index) const {
        return m_spriteSheet->GetClipName(index);
    }

    bool MothFlipbook::GetClipDesc(std::string_view name, moth_ui::IFlipbook::ClipDesc& outDesc) const {
        graphics::SpriteSheet::ClipDesc internal;
        if (!m_spriteSheet->GetClipDesc(name, internal)) {
            return false;
        }
        outDesc.loop = ToMothLoopType(internal.loop);
        outDesc.frames.clear();
        outDesc.frames.reserve(internal.frames.size());
        for (auto const& step : internal.frames) {
            moth_ui::IFlipbook::ClipFrame f;
            f.frameIndex = step.frameIndex;
            f.durationMs = step.durationMs;
            outDesc.frames.push_back(f);
        }
        return true;
    }
}
