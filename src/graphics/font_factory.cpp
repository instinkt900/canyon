#include "graphics/font_factory.h"
#include "graphics/context.h"

namespace graphics {
    FontFactory::FontFactory(Context& context)
        : m_context(context) {
    }

    void FontFactory::ClearFonts() {
        m_fonts.clear();
    }

    std::shared_ptr<graphics::IFont> FontFactory::GetFont(char const* name, int size) {
        auto fileIt = m_fonts.find(name);
        if (fileIt != m_fonts.end()) {
            auto sizeMap = fileIt->second;
            auto sizeIt = sizeMap.find(size);
            if (sizeIt != sizeMap.end()) {
                return sizeIt->second;
            }
        }
        std::shared_ptr<graphics::IFont> font = m_context.FontFromFile(name, size);

        auto sizeMap = m_fonts[name];
        sizeMap[size] = font;

        return font;
    }
}
