#include "graphics/sdl/sdl_context.h"
#include "graphics/sdl/sdl_image.h"
#include "graphics/sdl/sdl_texture.h"

namespace graphics::sdl {
    std::unique_ptr<ITexture> Context::TextureFromFile(std::filesystem::path const& path) {
        auto const& sdlContext = static_cast<Context const&>(*this);
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
}
