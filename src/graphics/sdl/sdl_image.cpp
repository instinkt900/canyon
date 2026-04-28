#include "common.h"
#include "moth_graphics/graphics/sdl/sdl_surface_context.h"
#include "moth_graphics/graphics/sdl/sdl_image.h"

namespace moth_graphics::graphics::sdl {
    Image::Image(std::shared_ptr<Texture> texture)
        : m_texture(texture) {
        sourceRect = IntRect{ { 0, 0 }, { m_texture->GetWidth(), m_texture->GetHeight() } };
    }

    Image::Image(std::shared_ptr<Texture> texture, IntRect const& sourceRect)
        : m_texture(texture)
        , sourceRect(sourceRect) {
    }

    int Image::GetWidth() const {
        return sourceRect.bottomRight.x - sourceRect.topLeft.x;
    }

    int Image::GetHeight() const {
        return sourceRect.bottomRight.y - sourceRect.topLeft.y;
    }

    std::shared_ptr<ITexture> Image::GetTexture() const {
        return m_texture;
    }

    void Image::ImGui(moth_graphics::IntVec2 const& size, moth_graphics::FloatVec2 const& uv0, moth_graphics::FloatVec2 const& uv1) const {
        ImGui::Image(m_texture ? m_texture->GetSDLTexture()->GetImpl() : nullptr,
                     ImVec2(static_cast<float>(size.x), static_cast<float>(size.y)),
                     ImVec2(uv0.x, uv0.y),
                     ImVec2(uv1.x, uv1.y));
    }

    std::unique_ptr<Image> Image::Load(SurfaceContext& context, std::filesystem::path const& path) {
        if (auto texture = Texture::FromFile(context.GetRenderer(), path)) {
            auto sdlTexture = std::unique_ptr<Texture>(static_cast<Texture*>(texture.release()));
            return std::make_unique<Image>(std::move(sdlTexture));
        }
        return nullptr;
    }
}
