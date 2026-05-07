#include "common.h"
#include "sdl_surface_context.h"

namespace moth_graphics::graphics::sdl {
    SurfaceContext::SurfaceContext(Context& context, SDL_Renderer* renderer)
        : m_context(context)
        , m_renderer(renderer)
        , m_assetContext(*this) {
    }
}
