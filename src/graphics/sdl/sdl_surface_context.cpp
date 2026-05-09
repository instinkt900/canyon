#include "common.h"
#include "moth_graphics/graphics/sdl/sdl_surface_context.h"

namespace moth_graphics::graphics::sdl {
    SurfaceContext::SurfaceContext(SDL_Renderer* renderer)
        : m_renderer(renderer)
        , m_assetContext(*this) {
    }
}
