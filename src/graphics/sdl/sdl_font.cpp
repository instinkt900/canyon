#include "canyon.h"
#include "graphics/sdl/sdl_font.h"

namespace graphics::sdl {
    Font::Font(CachedFontRef fontObj)
        : m_fontObj(fontObj) {
    }

    std::unique_ptr<IFont> Font::LoadFont(SDL_Renderer& renderer, const std::filesystem::path &path, int size) {
        SDL_Color defaultColor{ 0x00, 0x00, 0x00, 0xFF };
        return std::make_unique<Font>(CreateCachedFontRef(&renderer, path.string().c_str(), size, defaultColor, TTF_STYLE_NORMAL));
    }
}
