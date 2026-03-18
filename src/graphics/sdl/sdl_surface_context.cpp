#include "common.h"
#include "canyon/graphics/sdl/sdl_surface_context.h"

namespace canyon::graphics::sdl {
    SurfaceContext::SurfaceContext(Context& context, SDL_Renderer* renderer)
        : m_context(context)
        , m_renderer(renderer)
        , m_assetContext(*this) {
    }
}
