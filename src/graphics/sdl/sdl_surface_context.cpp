#include <canyon.h>
#include "graphics/sdl/sdl_surface_context.h"
#include "graphics/sdl/sdl_font.h"
#include "graphics/sdl/sdl_image.h"
#include "graphics/sdl/sdl_texture.h"

namespace graphics::sdl {
    SurfaceContext::SurfaceContext(Context& context)
        : m_context(context) {
    }

    std::unique_ptr<IImage> SurfaceContext::NewImage(std::shared_ptr<ITexture> texture) {
        auto sdlTexture = std::dynamic_pointer_cast<Texture>(texture);
        return std::make_unique<Image>(sdlTexture);
    }

    std::unique_ptr<IImage> SurfaceContext::NewImage(std::shared_ptr<ITexture> texture, IntRect const& sourceRect) {
        auto sdlTexture = std::dynamic_pointer_cast<Texture>(texture);
        return std::make_unique<Image>(sdlTexture, sourceRect);
    }

    std::unique_ptr<IFont> SurfaceContext::FontFromFile(std::filesystem::path const& path, uint32_t size) {
        return Font::Load(*m_renderer, path, size);
    }

    std::unique_ptr<ITexture> SurfaceContext::TextureFromFile(std::filesystem::path const& path) {
        return Texture::FromFile(m_renderer, path);
    }
}
