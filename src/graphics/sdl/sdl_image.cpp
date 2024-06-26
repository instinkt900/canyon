#include "canyon.h"
#include "graphics/sdl/sdl_image.h"
#include "graphics/sdl/smart_sdl.h"

namespace graphics::sdl {
    Image::Image(TextureRef texture)
        : m_texture(texture) {
        SDL_QueryTexture(texture->GetImpl(), NULL, NULL, &m_textureDimensions.x, &m_textureDimensions.y);
        m_sourceRect = IntRect{ { 0, 0 }, m_textureDimensions };
    }

    Image::Image(TextureRef texture, IntVec2 const& textureDimensions, IntRect const& sourceRect)
        : m_texture(texture)
        , m_textureDimensions(textureDimensions)
        , m_sourceRect(sourceRect) {
    }

    int Image::GetWidth() const {
        return m_sourceRect.bottomRight.x - m_sourceRect.topLeft.x;
    }

    int Image::GetHeight() const {
        return m_sourceRect.bottomRight.y - m_sourceRect.topLeft.y;
    }

    moth_ui::IntVec2 Image::GetDimensions() const {
        return { GetWidth(), GetHeight() };
    }

    void Image::ImGui(moth_ui::IntVec2 const& size, moth_ui::FloatVec2 const& uv0, moth_ui::FloatVec2 const& uv1) const {
        ImGui::Image(m_texture ? m_texture->GetImpl() : nullptr,
                     ImVec2(static_cast<float>(size.x), static_cast<float>(size.y)),
                     ImVec2(uv0.x, uv0.y),
                     ImVec2(uv1.x, uv1.y));
    }

    std::unique_ptr<Image> Image::Load(SDL_Renderer& renderer, std::filesystem::path const& path) {
        if (auto texture = CreateTextureRef(&renderer, path)) {
            IntVec2 textureDimensions{};
            SDL_QueryTexture(texture->GetImpl(), NULL, NULL, &textureDimensions.x, &textureDimensions.y);
            IntRect sourceRect{ { 0, 0 }, textureDimensions };
            return std::make_unique<Image>(texture, textureDimensions, sourceRect);
        }
        return nullptr;
    }

}
