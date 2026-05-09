#include "common.h"
#include "moth_graphics/graphics/sdl/sdl_surface_context.h"

#include <stdexcept>

namespace moth_graphics::graphics::sdl {
    SurfaceContext::SurfaceContext(SDL_Renderer* renderer)
        : m_renderer(renderer)
        , m_assetContext(*this) {
        if (m_renderer == nullptr) {
            throw std::invalid_argument("SDL: SurfaceContext renderer must not be null");
        }
    }
}
