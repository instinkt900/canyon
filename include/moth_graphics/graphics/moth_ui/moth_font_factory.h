#pragma once

#include "moth_graphics/graphics/font_factory.h"

#include <moth_ui/font_factory.h>
#include <moth_ui/graphics/ifont.h>

#include <memory>

namespace moth_graphics::graphics {
    class MothFontFactory : public moth_ui::FontFactory {
    public:
        explicit MothFontFactory(moth_graphics::graphics::FontFactory& factoryImpl);
        ~MothFontFactory() override = default;

        void ClearFonts() override;
        std::shared_ptr<moth_ui::IFont> GetFont(std::string const& name, int size) override;

    private:
        moth_graphics::graphics::FontFactory& m_factoryImpl;
    };
}
