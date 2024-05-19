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


    void Image::ImGui(moth_ui::IntVec2 const& size, moth_ui::FloatVec2 const& uv0, moth_ui::FloatVec2 const& uv1) const {
        ImGui::Image(m_texture ? m_texture->GetSDLTexture()->GetImpl(): nullptr,
                     ImVec2(static_cast<float>(size.x), static_cast<float>(size.y)),
                     ImVec2(uv0.x, uv0.y),
                     ImVec2(uv1.x, uv1.y));
    }

    std::unique_ptr<Image> Image::Load(SDL_Renderer& renderer, std::filesystem::path const& path) {
        if (auto texture = Texture::Load(renderer, path)) {
            return std::make_unique<Image>(std::shared_ptr<Texture>(texture.release()));
        }
        return nullptr;
    }
}
