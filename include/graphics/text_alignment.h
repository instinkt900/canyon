#pragma once

#include <moth_ui/text_alignment.h>
namespace graphics {
    enum class TextHorizAlignment {
        Left,
        Center,
        Right
    };

    enum class TextVertAlignment {
        Top,
        Middle,
        Bottom
    };

    inline TextHorizAlignment FromMothUI(moth_ui::TextHorizAlignment a) {
        if (a == moth_ui::TextHorizAlignment::Right) return TextHorizAlignment::Right;
        if (a == moth_ui::TextHorizAlignment::Center) return TextHorizAlignment::Center;
        return TextHorizAlignment::Left;
    }

    inline TextVertAlignment FromMothUI(moth_ui::TextVertAlignment a) {
        if (a == moth_ui::TextVertAlignment::Middle) return TextVertAlignment::Middle;
        if (a == moth_ui::TextVertAlignment::Bottom) return TextVertAlignment::Bottom;
        return TextVertAlignment::Top;
    }
}

