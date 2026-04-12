#pragma once

#include "moth_graphics/graphics/spritesheet.h"
#include "moth_graphics/graphics/moth_ui/moth_image.h"

#include <moth_ui/graphics/iflipbook.h>

#include <memory>

namespace moth_graphics::graphics {
    class MothFlipbook : public moth_ui::IFlipbook {
    public:
        explicit MothFlipbook(std::shared_ptr<moth_graphics::graphics::SpriteSheet> spriteSheet);
        ~MothFlipbook() override = default;

        moth_ui::IImage const& GetImage() const override;
        int GetFrameCount() const override;
        bool GetFrameDesc(int index, moth_ui::IFlipbook::FrameDesc& outDesc) const override;
        int GetClipCount() const override;
        std::string_view GetClipName(int index) const override;
        bool GetClipDesc(std::string_view name, moth_ui::IFlipbook::ClipDesc& outDesc) const override;

    private:
        std::shared_ptr<moth_graphics::graphics::SpriteSheet> m_spriteSheet;
        MothImage m_image;
    };
}
