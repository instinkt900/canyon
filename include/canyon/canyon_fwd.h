#pragma once

// Enums cannot be forward-declared without knowing their underlying type,
// so the lightweight enum-only headers are included directly.
#include "canyon/graphics/blend_mode.h"
#include "canyon/graphics/image_scale_type.h"
#include "canyon/graphics/text_alignment.h"
#include "canyon/graphics/texture_address_mode.h"
#include "canyon/graphics/texture_filter.h"

// Rect and vector types are re-exported aliases from moth_ui; pull in its
// forward declarations so canyon consumers can use them without a full include.
#include "moth_ui/moth_ui_fwd.h"

namespace canyon {

    // -------------------------------------------------------------------------
    // Events
    // -------------------------------------------------------------------------
    class EventEmitter;

    // -------------------------------------------------------------------------
    // Utils
    // -------------------------------------------------------------------------
    class Ticker;

} // namespace canyon

namespace canyon::platform {

    // -------------------------------------------------------------------------
    // Platform
    // -------------------------------------------------------------------------
    class Application;
    class IPlatform;
    class Window;

} // namespace canyon::platform

namespace canyon::graphics {

    // -------------------------------------------------------------------------
    // Graphics interfaces
    // -------------------------------------------------------------------------
    class IGraphics;
    class IImage;
    class IFont;
    class ITarget;
    class ITexture;

    // -------------------------------------------------------------------------
    // Graphics core
    // -------------------------------------------------------------------------
    class Context;
    class SurfaceContext;
    class FontFactory;
    class ImageFactory;

    // -------------------------------------------------------------------------
    // moth_ui bridge
    // -------------------------------------------------------------------------
    class MothRenderer;
    class MothImage;
    class MothFont;
    class MothImageFactory;
    class MothFontFactory;

} // namespace canyon::graphics
