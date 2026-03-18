#include "common.h"
#include "canyon/graphics/font_factory.h"
namespace canyon::graphics {
    FontFactory::FontFactory(AssetContext& context)
        : m_context(context) {
    }

    void FontFactory::ClearFonts() {
        m_fonts.clear();
    }

    std::shared_ptr<IFont> FontFactory::GetFont(std::string const& name, int size) {
        auto fileIt = m_fonts.find(name);
        if (fileIt != m_fonts.end()) {
            auto const& sizeMap = fileIt->second;
            auto sizeIt = sizeMap.find(size);
            if (sizeIt != sizeMap.end()) {
                return sizeIt->second;
            }
        }

        std::shared_ptr<IFont> font = m_context.FontFromFile(name, size);
        auto& sizeMap = m_fonts[name];
        sizeMap[size] = font;
        return font;
    }
}
