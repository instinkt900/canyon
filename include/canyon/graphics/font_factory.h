#pragma once

#include "canyon/graphics/igraphics.h"
#include "canyon/graphics/ifont.h"

#include <map>
#include <string>
#include <memory>

namespace canyon::graphics {
    class FontFactory {
    public:
        FontFactory(SurfaceContext& context);
        virtual ~FontFactory() = default;

        void ClearFonts();
        std::shared_ptr<IFont> GetFont(std::string const& name, int size);

    private:
        SurfaceContext& m_context;
        std::map<std::string, std::map<int, std::shared_ptr<IFont>>> m_fonts;
    };
}
