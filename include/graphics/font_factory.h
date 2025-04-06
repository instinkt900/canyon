#pragma once

#include "graphics/igraphics.h"
#include "graphics/ifont.h"

#include <map>
#include <string>
#include <memory>

namespace graphics {
    class FontFactory {
    public:
        FontFactory(SurfaceContext& context);
        virtual ~FontFactory() = default;

        void ClearFonts();
        std::shared_ptr<IFont> GetFont(char const* name, int size);

    private:
        SurfaceContext& m_context;
        std::map<std::string, std::map<int, std::shared_ptr<graphics::IFont>>> m_fonts;
    };
}
