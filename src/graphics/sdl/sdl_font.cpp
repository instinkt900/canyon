#include "common.h"
#include "moth_graphics/graphics/sdl/sdl_font.h"
#include "moth_graphics/graphics/sdl/SDL_FontCache.h"
#include <SDL_ttf.h>

namespace moth_graphics::graphics::sdl {
    Font::Font(CachedFontRef fontObj)
        : m_fontObj(fontObj) {
    }

    IntVec2 Font::Measure(std::string_view text) const {
        std::string const str(text);
        return {
            static_cast<int>(FC_GetWidth(m_fontObj.get(), "%s", str.c_str())), // NOLINT(cppcoreguidelines-pro-type-vararg)
            static_cast<int>(FC_GetHeight(m_fontObj.get(), "%s", str.c_str())) // NOLINT(cppcoreguidelines-pro-type-vararg)
        };
    }

    std::unique_ptr<IFont> Font::Load(SDL_Renderer& renderer, const std::filesystem::path& path, int size) {
        SDL_Color defaultColor{ 0x00, 0x00, 0x00, 0xFF };
        return std::make_unique<Font>(CreateCachedFontRef(&renderer, path.string().c_str(), size, defaultColor, TTF_STYLE_NORMAL));
    }
}
