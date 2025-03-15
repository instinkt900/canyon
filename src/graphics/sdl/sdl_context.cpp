#include "graphics/sdl/sdl_context.h"
#include "graphics/sdl/sdl_font.h"
#include "graphics/sdl/sdl_image.h"
#include "graphics/sdl/sdl_texture.h"

namespace graphics::sdl {
    std::unique_ptr<ITexture> Context::TextureFromFile(std::filesystem::path const& path) {
        auto& sdlContext = static_cast<Context&>(*this);
        return Texture::FromFile(sdlContext, path);
    }

    std::unique_ptr<IImage> Context::NewImage(std::shared_ptr<ITexture> texture) {
        auto sdlTexture = std::dynamic_pointer_cast<Texture>(texture);
        return std::make_unique<Image>(sdlTexture);
    }

    std::unique_ptr<IImage> Context::NewImage(std::shared_ptr<ITexture> texture, IntRect const& sourceRect) {
        auto sdlTexture = std::dynamic_pointer_cast<Texture>(texture);
        return std::make_unique<Image>(sdlTexture, sourceRect);
    }

    std::unique_ptr<IFont> Context::FontFromFile(std::filesystem::path const& path, int size, IGraphics& graphics) {
        return Font::Load(*m_renderer, path, size);
    }
}
