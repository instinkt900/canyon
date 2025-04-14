#pragma once

#include "canyon/graphics/font_factory.h"

#include <moth_ui/font_factory.h>
#include <moth_ui/ifont.h>

#include <memory>

namespace canyon::graphics {
    class MothFontFactory : public moth_ui::FontFactory {
    public:
        MothFontFactory(std::shared_ptr<canyon::graphics::FontFactory> factoryImpl);
        virtual ~MothFontFactory() = default;

        void ClearFonts() override;
        std::shared_ptr<moth_ui::IFont> GetFont(std::string const& name, int size) override;

    private:
        std::shared_ptr<canyon::graphics::FontFactory> m_factoryImpl;
    };
}
