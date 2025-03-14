#include "graphics/font_factory.h"
#include "graphics/context.h"

namespace graphics {
    FontFactory::FontFactory(graphics::IGraphics& graphics)
        : m_graphics(graphics) {
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
        std::shared_ptr<graphics::IFont> font = m_graphics.GetContext().FontFromFile(name, size, m_graphics);

        auto sizeMap = m_fonts[name];
        sizeMap[size] = font;

        return font;
    }
}
