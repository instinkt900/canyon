#include "common.h"
#include "canyon/graphics/moth_ui/moth_font_factory.h"
#include "canyon/graphics/font_factory.h"
#include "canyon/graphics/moth_ui/moth_font.h"

namespace canyon::graphics {
    MothFontFactory::MothFontFactory(std::shared_ptr<canyon::graphics::FontFactory> factoryImpl)
        : m_factoryImpl(factoryImpl) {
    }

    void MothFontFactory::ClearFonts() {
        m_factoryImpl->ClearFonts();
    }

    std::shared_ptr<moth_ui::IFont> MothFontFactory::GetFont(std::string const& name, int size) {
        auto it = m_fontPaths.find(name);
        if (it != m_fontPaths.end()) {
           auto const font = m_factoryImpl->GetFont(it->second.string(), size);
           return std::make_shared<MothFont>(font);
        }
        return nullptr;
    }
}
