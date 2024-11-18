#include "graphics/blend_mode.h"
#include "moth_ui/blend_mode.h"

namespace graphics {
    inline BlendMode FromMothUI(moth_ui::BlendMode blend) {
        switch (blend) {
            case moth_ui::BlendMode::Modulate: { return BlendMode::Modulate; }
            case moth_ui::BlendMode::Multiply: { return BlendMode::Multiply; }
            case moth_ui::BlendMode::Add: { return BlendMode::Add; }
            case moth_ui::BlendMode::Alpha: { return BlendMode::Alpha; }
            case moth_ui::BlendMode::Replace: { return BlendMode::Replace; }
            default: { return BlendMode::Invalid; }
        }
    }
}


