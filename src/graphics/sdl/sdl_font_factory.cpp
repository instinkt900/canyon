#include "canyon.h"
#include "graphics/sdl/sdl_font_factory.h"
#include "graphics/sdl/sdl_font.h"

namespace graphics::sdl {
    FontFactory::FontFactory(SDL_Renderer& renderer)
        : m_renderer(renderer) {
    }

    std::shared_ptr<IFont> FontFactory::GetFont(char const* name, int size) {
        assert(!m_fontPaths.empty() && "No known fonts.");
        auto const it = m_fontPaths.find(name);
        if (std::end(m_fontPaths) == it) {
            return GetDefaultFont(size);
        }
        SDL_Color defaultColor{ 0x00, 0x00, 0x00, 0xFF };
        return std::make_unique<Font>(CreateCachedFontRef(&m_renderer, it->second.string().c_str(), size, defaultColor, TTF_STYLE_NORMAL));
    }
}
