// Pins enum values for BlendMode, ImageScaleType, TextAlignment,
// SpriteSheet::LoopType, and CanyonEventType.  Any rename or reorder will
// fail to compile.

#include "moth_graphics/moth_graphics.h"

#include <catch2/catch_all.hpp>

using namespace moth_graphics;
using namespace moth_graphics::graphics;

TEST_CASE("BlendMode enum values are stable", "[api][enums][blend_mode]") {
    static_assert(BlendMode::Invalid  != BlendMode::Replace);
    static_assert(BlendMode::Replace  != BlendMode::Alpha);
    static_assert(BlendMode::Alpha    != BlendMode::Add);
    static_assert(BlendMode::Add      != BlendMode::Multiply);
    static_assert(BlendMode::Multiply != BlendMode::Modulate);
    SUCCEED();
}

TEST_CASE("ImageScaleType enum values are stable", "[api][enums][image_scale_type]") {
    static_assert(ImageScaleType::Stretch  != ImageScaleType::Tile);
    static_assert(ImageScaleType::Tile     != ImageScaleType::NineSlice);
    SUCCEED();
}

TEST_CASE("TextHorizAlignment enum values are stable", "[api][enums][text_alignment]") {
    static_assert(TextHorizAlignment::Left   != TextHorizAlignment::Center);
    static_assert(TextHorizAlignment::Center != TextHorizAlignment::Right);
    SUCCEED();
}

TEST_CASE("TextVertAlignment enum values are stable", "[api][enums][text_alignment]") {
    static_assert(TextVertAlignment::Top    != TextVertAlignment::Middle);
    static_assert(TextVertAlignment::Middle != TextVertAlignment::Bottom);
    SUCCEED();
}

TEST_CASE("SpriteSheet::LoopType enum values are stable", "[api][enums][loop_type]") {
    static_assert(SpriteSheet::LoopType::Stop  != SpriteSheet::LoopType::Reset);
    static_assert(SpriteSheet::LoopType::Reset != SpriteSheet::LoopType::Loop);
    SUCCEED();
}

TEST_CASE("CanyonEventType constants are stable and ordered", "[api][enums][canyon_events]") {
    static_assert(EVENTTYPE_RENDERDEVICERESET != EVENTTYPE_RENDERTARGETRESET);
    static_assert(EVENTTYPE_RENDERTARGETRESET != EVENTTYPE_WINDOWSIZE);
    static_assert(EVENTTYPE_WINDOWSIZE        != EVENTTYPE_REQUEST_QUIT);
    static_assert(EVENTTYPE_REQUEST_QUIT      != EVENTTYPE_QUIT);
    // User extension point sits above all system event types.
    static_assert(EVENTTYPE_CANYONUSER0 > EVENTTYPE_QUIT);
    SUCCEED();
}
