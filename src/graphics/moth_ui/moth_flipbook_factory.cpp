#include "common.h"
#include "moth_graphics/graphics/moth_ui/moth_flipbook_factory.h"
#include "moth_graphics/graphics/moth_ui/moth_flipbook.h"

namespace moth_graphics::graphics {
    MothFlipbookFactory::MothFlipbookFactory(SpriteSheetFactory& factoryImpl)
        : m_factoryImpl(factoryImpl) {
    }

    std::unique_ptr<moth_ui::IFlipbook> MothFlipbookFactory::GetFlipbook(std::filesystem::path const& path) {
        auto spriteSheet = m_factoryImpl.GetSpriteSheet(path);
        if (!spriteSheet) {
            return nullptr;
        }
        return std::make_unique<MothFlipbook>(std::move(spriteSheet));
    }
}
