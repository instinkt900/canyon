#include "common.h"
#include "sdl_target.h"

namespace moth_graphics::graphics::sdl {
    Target::Target(std::shared_ptr<Texture> texture)
        : m_texture(std::move(texture)) {
    }

    Image Target::GetImage() const {
        return Image{ m_texture };
    }
}
