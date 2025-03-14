#include "graphics/moth_ui/moth_font_factory.h"
#include "graphics/context.h"
#include "graphics/moth_ui/moth_font.h"

namespace graphics {
    MothFontFactory::MothFontFactory(graphics::IGraphics& graphics)
        : m_graphics(graphics) {
    }

    void MothFontFactory::ClearFonts() {
        m_fonts.clear();
    }

    std::shared_ptr<moth_ui::IFont> MothFontFactory::GetFont(char const* name, int size) {
        auto fileIt = m_fonts.find(name);
        if (fileIt != m_fonts.end()) {
            auto sizeMap = fileIt->second;
            auto sizeIt = sizeMap.find(size);
            if (sizeIt != sizeMap.end()) {
                return sizeIt->second;
            }
        }
        auto font = m_graphics.GetContext().FontFromFile(name, size, m_graphics);
        auto mothFont = std::make_shared<MothFont>(font);

        auto sizeMap = m_fonts[name];
        sizeMap[size] = mothFont;

        return mothFont;
    }
}
