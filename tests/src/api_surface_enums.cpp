// Pins enum values for BlendMode, ImageScaleType, TextAlignment,
// SpriteSheet::LoopType, and CanyonEventType.  Any rename, reorder, or
// renumber will fail to compile.

#include "moth_graphics/moth_graphics.h"

#include <catch2/catch_all.hpp>
#include <type_traits>

using namespace moth_graphics;
using namespace moth_graphics::graphics;

TEST_CASE("BlendMode enum values are stable", "[api][enums][blend_mode]") {
    using U = std::underlying_type_t<BlendMode>;
    static_assert(static_cast<U>(BlendMode::Invalid)  == -1);
    static_assert(static_cast<U>(BlendMode::Replace)  ==  0);
    static_assert(static_cast<U>(BlendMode::Alpha)    ==  1);
    static_assert(static_cast<U>(BlendMode::Add)      ==  2);
    static_assert(static_cast<U>(BlendMode::Multiply) ==  3);
    static_assert(static_cast<U>(BlendMode::Modulate) ==  4);
    SUCCEED();
}

TEST_CASE("ImageScaleType enum values are stable", "[api][enums][image_scale_type]") {
    using U = std::underlying_type_t<ImageScaleType>;
    static_assert(static_cast<U>(ImageScaleType::Stretch)   == 0);
    static_assert(static_cast<U>(ImageScaleType::Tile)      == 1);
    static_assert(static_cast<U>(ImageScaleType::NineSlice) == 2);
    SUCCEED();
}

TEST_CASE("TextHorizAlignment enum values are stable", "[api][enums][text_alignment]") {
    using U = std::underlying_type_t<TextHorizAlignment>;
    static_assert(static_cast<U>(TextHorizAlignment::Left)   == 0);
    static_assert(static_cast<U>(TextHorizAlignment::Center) == 1);
    static_assert(static_cast<U>(TextHorizAlignment::Right)  == 2);
    SUCCEED();
}

TEST_CASE("TextVertAlignment enum values are stable", "[api][enums][text_alignment]") {
    using U = std::underlying_type_t<TextVertAlignment>;
    static_assert(static_cast<U>(TextVertAlignment::Top)    == 0);
    static_assert(static_cast<U>(TextVertAlignment::Middle) == 1);
    static_assert(static_cast<U>(TextVertAlignment::Bottom) == 2);
    SUCCEED();
}

TEST_CASE("SpriteSheet::LoopType enum values are stable", "[api][enums][loop_type]") {
    using U = std::underlying_type_t<SpriteSheet::LoopType>;
    static_assert(static_cast<U>(SpriteSheet::LoopType::Stop)  == 0);
    static_assert(static_cast<U>(SpriteSheet::LoopType::Reset) == 1);
    static_assert(static_cast<U>(SpriteSheet::LoopType::Loop)  == 2);
    SUCCEED();
}

TEST_CASE("CanyonEventType constants are stable", "[api][enums][canyon_events]") {
    // moth_ui::EVENTTYPE_USER0 == 1000; canyon types are offset from there.
    static_assert(EVENTTYPE_RENDERDEVICERESET == 1000);
    static_assert(EVENTTYPE_RENDERTARGETRESET == 1001);
    static_assert(EVENTTYPE_WINDOWSIZE        == 1002);
    static_assert(EVENTTYPE_REQUEST_QUIT      == 1003);
    static_assert(EVENTTYPE_QUIT              == 1004);
    // User extension point starts 100 above the base.
    static_assert(EVENTTYPE_CANYONUSER0       == 1100);
    SUCCEED();
}
