#include "common.h"
#include "moth_graphics/graphics/moth_ui/moth_font_factory.h"
#include "moth_graphics/graphics/font_factory.h"
#include "moth_graphics/graphics/moth_ui/moth_font.h"

namespace moth_graphics::graphics {
    MothFontFactory::MothFontFactory(moth_graphics::graphics::FontFactory& factoryImpl)
        : m_factoryImpl(factoryImpl) {
    }

    void MothFontFactory::ClearFonts() {
        m_factoryImpl.ClearFonts();
    }

    std::shared_ptr<moth_ui::IFont> MothFontFactory::GetFont(std::string const& name, int size) {
        auto it = m_fontPaths.find(name);
        if (it != m_fontPaths.end()) {
           auto const font = m_factoryImpl.GetFont(it->second.string(), size);
           return std::make_shared<MothFont>(font);
        }
        spdlog::warn("MothFontFactory: font '{}' not registered (call AddFont first)", name);
        return nullptr;
    }
}
