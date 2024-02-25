#pragma once

#include "graphics/ifont.h"
#include "smart_sdl.h"

namespace graphics::sdl {
    class Font : public IFont {
    public:
        explicit Font(CachedFontRef fontObj);
        virtual ~Font() = default;

        CachedFontRef GetFontObj() const {
            return m_fontObj;
        }

    private:
        CachedFontRef m_fontObj;
    };
}
