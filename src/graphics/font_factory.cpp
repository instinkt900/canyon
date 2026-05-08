#include "common.h"
#include "moth_graphics/graphics/font_factory.h"
namespace moth_graphics::graphics {
    FontFactory::FontFactory(AssetContext& context)
        : m_context(context) {
    }

    void FontFactory::ClearFonts() {
        m_fonts.clear();
    }

    std::shared_ptr<IFont> FontFactory::GetFont(std::string const& path, int size) {
        auto fileIt = m_fonts.find(path);
        if (fileIt != m_fonts.end()) {
            auto const& sizeMap = fileIt->second;
            auto sizeIt = sizeMap.find(size);
            if (sizeIt != sizeMap.end()) {
                return sizeIt->second;
            }
        }

        std::shared_ptr<IFont> font = m_context.FontFromFile(path, size);
        if (font) {
            m_fonts[path][size] = font;
        }
        return font;
    }
}
