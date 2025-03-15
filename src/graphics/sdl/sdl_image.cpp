#include "canyon.h"
#include "graphics/sdl/sdl_image.h"

namespace graphics::sdl {
    Image::Image(std::shared_ptr<Texture> texture)
        : m_texture(texture) {
        m_sourceRect = IntRect{ { 0, 0 }, { m_texture->GetWidth(), m_texture->GetHeight() } };
    }

    Image::Image(std::shared_ptr<Texture> texture, IntRect const& sourceRect)
        : m_texture(texture)
        , m_sourceRect(sourceRect) {
    }

    int Image::GetWidth() const {
        return m_sourceRect.bottomRight.x - m_sourceRect.topLeft.x;
    }

    int Image::GetHeight() const {
        return m_sourceRect.bottomRight.y - m_sourceRect.topLeft.y;
    }

    std::unique_ptr<Image> Image::Load(graphics::Context& context, std::filesystem::path const& path) {
        if (auto texture = context.TextureFromFile(path)) {
            auto sdlTexture = std::unique_ptr<Texture>(static_cast<Texture*>(texture.release()));
            return std::make_unique<Image>(std::move(sdlTexture));
        }
        return nullptr;
    }
}
