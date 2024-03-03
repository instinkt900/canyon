#include "graphics/vulkan/vulkan_font_cache.h"

namespace graphics::vulkan {
    FontCache::FontCache(Context& context, Graphics& graphics)
        : m_context(context)
        , m_graphics(graphics) {
    }

    FontCache ::~FontCache() {
    }

    std::shared_ptr<IFont> FontCache::GetFont(std::string const& path, int size) {
        auto fontPathIt = m_fonts.find(path);
        if (std::end(m_fonts) == fontPathIt) {
            return LoadFont(path, size);
        }

        auto fontSizeIt = fontPathIt->second.find(size);
        if (std::end(fontPathIt->second) == fontSizeIt) {
            return LoadFont(path, size);
        }

        return fontSizeIt->second;
    }
    
    void FontCache::Clear() {
        m_fonts.clear();
    }

    std::shared_ptr<IFont> FontCache::LoadFont(std::string const& path, int size) {
        auto font = Font::Load(path.c_str(), size, m_context, m_graphics);

        auto fontPathIt = m_fonts.find(path);
        if (std::end(m_fonts) == fontPathIt) {
            auto insertResult = m_fonts.insert(std::make_pair(path, FontSizes{}));
            if (!insertResult.second) {
                return nullptr;
            }
            fontPathIt = insertResult.first;
        }

        auto insertResult = fontPathIt->second.insert(std::make_pair(size, font));
        if (!insertResult.second) {
            return nullptr;
        }

        return font;
    }
}
