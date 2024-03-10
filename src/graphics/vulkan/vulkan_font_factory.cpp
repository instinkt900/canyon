#include "canyon.h"
#include "graphics/vulkan/vulkan_font_factory.h"
#include "graphics/vulkan/vulkan_font.h"

namespace graphics::vulkan {
    FontFactory::FontFactory(Context& context, Graphics& graphics)
        : m_fontCache(context, graphics) {
    }

    void FontFactory::ClearFonts() {
        graphics::FontFactory::ClearFonts();
        m_fontCache.Clear();
    }

    std::shared_ptr<IFont> FontFactory::GetFont(char const* name, int size) {
        assert(!m_fontPaths.empty() && "No known fonts.");
        auto const it = m_fontPaths.find(name);
        if (std::end(m_fontPaths) == it) {
            return GetDefaultFont(size);
        }
        return m_fontCache.GetFont(it->second.string().c_str(), size);
    }
}
