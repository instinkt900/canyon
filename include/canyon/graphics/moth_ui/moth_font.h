#pragma once

#include "canyon/graphics/ifont.h"

#include <moth_ui/ifont.h>

#include <memory>

namespace canyon::graphics {
    class MothFont : public moth_ui::IFont {
    public:
        explicit MothFont(std::shared_ptr<graphics::IFont> internalFont)
            : m_font(internalFont) {}
        virtual ~MothFont() = default;

        std::shared_ptr<graphics::IFont> GetInternalFont() { return m_font; }

    private:
        std::shared_ptr<graphics::IFont> m_font;
    };
}
