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

    void MothFlipbook::GetSheetDesc(moth_ui::IFlipbook::SheetDesc& outDesc) const {
        graphics::SpriteSheet::SheetDesc internal;
        m_spriteSheet->GetSheetDesc(internal);
        outDesc.FrameDimensions = internal.FrameDimensions;
        outDesc.SheetCells      = internal.SheetCells;
        outDesc.MaxFrames       = internal.MaxFrames;
        outDesc.NumClips        = internal.NumClips;
    }

    std::string_view MothFlipbook::GetClipName(int index) const {
        return m_spriteSheet->GetClipName(index);
    }

    bool MothFlipbook::GetClipDesc(std::string_view name, moth_ui::IFlipbook::ClipDesc& outDesc) const {
        graphics::SpriteSheet::ClipDesc internal;
        if (!m_spriteSheet->GetClipDesc(name, internal)) {
            return false;
        }
        outDesc.Start = internal.Start;
        outDesc.End   = internal.End;
        outDesc.FPS   = internal.FPS;
        outDesc.Loop  = ToMothLoopType(internal.Loop);
        return true;
    }
}
