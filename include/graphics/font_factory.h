#pragma once

#include "graphics/igraphics.h"

namespace graphics {
    class FontFactory {
    public:
        FontFactory(graphics::IGraphics& graphics);
        virtual ~FontFactory() = default;

        void ClearFonts();
        std::shared_ptr<graphics::IFont> GetFont(char const* name, int size);

    private:
        graphics::IGraphics& m_graphics;
        std::map<std::string, std::map<int, std::shared_ptr<graphics::IFont>>> m_fonts;
    };
}

