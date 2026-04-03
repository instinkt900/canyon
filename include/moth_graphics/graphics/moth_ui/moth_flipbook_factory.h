#pragma once

#include "moth_graphics/graphics/spritesheet_factory.h"

#include <moth_ui/iflipbook_factory.h>

#include <filesystem>
#include <memory>

namespace moth_graphics::graphics {
    class MothFlipbookFactory : public moth_ui::IFlipbookFactory {
    public:
        explicit MothFlipbookFactory(SpriteSheetFactory& factoryImpl);
        ~MothFlipbookFactory() override = default;

        std::unique_ptr<moth_ui::IFlipbook> GetFlipbook(std::filesystem::path const& path) override;

    private:
        SpriteSheetFactory& m_factoryImpl;
    };
}
