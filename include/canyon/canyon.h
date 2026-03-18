#pragma once

// Export macro — currently a no-op (static library build).
// See canyon_fwd.h for forward declarations.
#if defined(_WIN32) || defined(__CYGWIN__)
  #if defined(CANYON_BUILD_SHARED)
    #if defined(CANYON_BUILD)
      #define CANYON_API __declspec(dllexport)
    #else
      #define CANYON_API __declspec(dllimport)
    #endif
  #else
    #define CANYON_API
  #endif
#else
  #if defined(CANYON_BUILD_SHARED)
    #if defined(CANYON_BUILD)
      #define CANYON_API __attribute__((visibility("default")))
    #else
      #define CANYON_API
    #endif
  #else
    #define CANYON_API
  #endif
#endif

// events
#include "canyon/events/canyon_events.h"
#include "canyon/events/event_emitter.h"
#include "canyon/events/event_window.h"

// graphics — interfaces and core
#include "canyon/graphics/blend_mode.h"
#include "canyon/graphics/color.h"
#include "canyon/graphics/context.h"
#include "canyon/graphics/font_factory.h"
#include "canyon/graphics/ifont.h"
#include "canyon/graphics/igraphics.h"
#include "canyon/graphics/iimage.h"
#include "canyon/graphics/image_factory.h"
#include "canyon/graphics/image_scale_type.h"
#include "canyon/graphics/itarget.h"
#include "canyon/graphics/itexture.h"
#include "canyon/graphics/surface_context.h"
#include "canyon/graphics/text_alignment.h"
#include "canyon/graphics/texture_address_mode.h"
#include "canyon/graphics/texture_filter.h"

// graphics — moth_ui bridge
#include "canyon/graphics/moth_ui/moth_font.h"
#include "canyon/graphics/moth_ui/moth_font_factory.h"
#include "canyon/graphics/moth_ui/moth_image.h"
#include "canyon/graphics/moth_ui/moth_image_factory.h"
#include "canyon/graphics/moth_ui/moth_renderer.h"
#include "canyon/graphics/moth_ui/utils.h"

// platform
#include "canyon/platform/application.h"
#include "canyon/platform/iplatform.h"
#include "canyon/platform/window.h"

// utils
#include "canyon/utils/math_utils.h"
#include "canyon/utils/moth_ui_format.h"
#include "canyon/utils/rect.h"
#include "canyon/utils/rect_format.h"
#include "canyon/utils/ticker.h"
#include "canyon/utils/vector.h"
