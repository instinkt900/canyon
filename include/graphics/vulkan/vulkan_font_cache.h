#pragma once

#include "vulkan_font.h"

namespace graphics::vulkan {
    class FontCache {
    public:
        FontCache(Context& context, Graphics& graphics);
        ~FontCache();

        std::shared_ptr<IFont> GetFont(std::string const& path, int size);
        void Clear();

    private:
        std::shared_ptr<IFont> LoadFont(std::string const& path, int size);

        Context& m_context;
        Graphics& m_graphics;
        using FontSizes = std::map<int, std::shared_ptr<Font>>;
        std::map<std::string, FontSizes> m_fonts;
    };
}
