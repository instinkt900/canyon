#include "graphics/moth_ui/moth_font_factory.h"
#include "graphics/font_factory.h"
#include "graphics/moth_ui/moth_font.h"

namespace graphics {
    MothFontFactory::MothFontFactory(std::shared_ptr<graphics::FontFactory> factoryImpl)
        : m_factoryImpl(factoryImpl) {
    }

    void MothFontFactory::ClearFonts() {
        m_factoryImpl->ClearFonts();
    }

    std::shared_ptr<moth_ui::IFont> MothFontFactory::GetFont(char const* name, int size) {
        auto font = m_factoryImpl->GetFont(name, size);
        return std::make_shared<MothFont>(font);
    }
}
