#pragma once

#include "moth_ui/blend_mode.h"

namespace graphics {
    enum class BlendMode {
        Invalid = -1,
        Replace,
        Alpha,
        Add,
        Multiply,
        Modulate
    };

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

