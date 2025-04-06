#pragma once

#include "graphics/font_factory.h"

#include <moth_ui/font_factory.h>
#include <moth_ui/ifont.h>

#include <memory>

namespace graphics {
    class MothFontFactory : public moth_ui::FontFactory {
    public:
        MothFontFactory(std::shared_ptr<graphics::FontFactory> factoryImpl);
        virtual ~MothFontFactory() = default;

        void ClearFonts() override;
        std::shared_ptr<moth_ui::IFont> GetFont(char const* name, int size) override;

    private:
        std::shared_ptr<graphics::FontFactory> m_factoryImpl;
    };
}
