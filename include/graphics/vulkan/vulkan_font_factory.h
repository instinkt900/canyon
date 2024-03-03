#pragma once

#include "graphics/font_factory.h"
#include "graphics/ifont.h"
#include "vulkan_context.h"
#include "vulkan_graphics.h"
#include "vulkan_font_cache.h"

namespace graphics::vulkan {
    class FontFactory : public graphics::FontFactory {
    public:
        FontFactory(Context& context, Graphics& graphics);
        virtual ~FontFactory() = default;

        void ClearFonts() override;

        std::shared_ptr<IFont> GetFont(char const* name, int size) override;

    private:
        FontCache m_fontCache;
    };
}
