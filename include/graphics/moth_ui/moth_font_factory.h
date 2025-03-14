#pragma once

#include "graphics/igraphics.h"
#include <moth_ui/font_factory.h>

namespace graphics {
    class MothFontFactory : public moth_ui::FontFactory {
    public:
        MothFontFactory(graphics::IGraphics& graphics);
        virtual ~MothFontFactory() = default;

        void ClearFonts() override;
        std::shared_ptr<moth_ui::IFont> GetFont(char const* name, int size) override;

    private:
        graphics::IGraphics& m_graphics;
        std::map<std::string, std::map<int, std::shared_ptr<moth_ui::IFont>>> m_fonts;
    };
}

