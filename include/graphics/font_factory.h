#pragma once

#include "graphics/igraphics.h"

namespace graphics {
    class FontFactory {
    public:
        FontFactory(Context& context);
        virtual ~FontFactory() = default;

        void ClearFonts();
        std::shared_ptr<graphics::IFont> GetFont(char const* name, int size);

    private:
        Context& m_context;
        std::map<std::string, std::map<int, std::shared_ptr<graphics::IFont>>> m_fonts;
    };
}

