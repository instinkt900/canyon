#pragma once

#include "moth_graphics/version.h"

// Static library — no export annotation needed.
#define MOTH_GRAPHICS_API

// events
// EventRenderDeviceReset and EventRenderTargetReset are also defined in event_window.h.
#include "moth_graphics/events/moth_graphics_events.h"
#include "moth_graphics/events/event_emitter.h"
#include "moth_graphics/events/event_window.h"

// graphics — interfaces and core
#include "moth_graphics/graphics/asset_context.h"
#include "moth_graphics/graphics/blend_mode.h"
#include "moth_graphics/graphics/color.h"
#include "moth_graphics/graphics/font_factory.h"
#include "moth_graphics/graphics/ifont.h"
#include "moth_graphics/graphics/igraphics.h"
#include "moth_graphics/graphics/image.h"
#include "moth_graphics/graphics/image_scale_type.h"
#include "moth_graphics/graphics/itarget.h"
#include "moth_graphics/graphics/itexture.h"
#include "moth_graphics/graphics/sprite.h"
#include "moth_graphics/graphics/spritesheet.h"
#include "moth_graphics/graphics/spritesheet_factory.h"
#include "moth_graphics/graphics/surface_context.h"
#include "moth_graphics/graphics/text_alignment.h"
#include "moth_graphics/graphics/texture_address_mode.h"
#include "moth_graphics/graphics/texture_factory.h"
#include "moth_graphics/graphics/texture_filter.h"

// graphics — moth_ui bridge
#include "moth_graphics/graphics/moth_ui/moth_flipbook.h"
#include "moth_graphics/graphics/moth_ui/moth_flipbook_factory.h"
#include "moth_graphics/graphics/moth_ui/moth_font.h"
#include "moth_graphics/graphics/moth_ui/moth_font_factory.h"
#include "moth_graphics/graphics/moth_ui/moth_image.h"
#include "moth_graphics/graphics/moth_ui/moth_image_factory.h"
#include "moth_graphics/graphics/moth_ui/moth_renderer.h"

// platform
#include "moth_graphics/platform/application.h"
#include "moth_graphics/platform/imgui_context.h"
#include "moth_graphics/platform/iplatform.h"
#include "moth_graphics/platform/window.h"

// utils
#include "moth_graphics/utils/math_utils.h"
#include "moth_graphics/utils/moth_ui_format.h"
#include "moth_graphics/utils/rect.h"
#include "moth_graphics/utils/ticker.h"
#include "moth_graphics/utils/transform.h"
#include "moth_graphics/utils/vector.h"
