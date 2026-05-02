#include "common.h"
#include "moth_graphics/graphics/vulkan/vulkan_image.h"
#include "moth_graphics/graphics/vulkan/vulkan_graphics.h"

namespace moth_graphics::graphics::vulkan {
    Image::Image(std::shared_ptr<Texture> texture)
    : m_texture(texture) 
    , m_sourceRect({{ 0, 0 }, { texture->GetWidth(), texture->GetHeight() }}) {

    }

    Image::Image(std::shared_ptr<Texture> texture, IntRect const& m_sourceRect)
        : m_texture(texture)
        , m_sourceRect(m_sourceRect) {
    }

    Image::~Image() {
    }

    int Image::GetWidth() const {
        return m_sourceRect.bottomRight.x - m_sourceRect.topLeft.x;
    }

    int Image::GetHeight() const {
        return m_sourceRect.bottomRight.y - m_sourceRect.topLeft.y;
    }

    std::shared_ptr<ITexture> Image::GetTexture() const {
        return m_texture;
    }

    void Image::ImGui(moth_graphics::IntVec2 const& size, moth_graphics::FloatVec2 const& uv0, moth_graphics::FloatVec2 const& uv1) const {
        if (m_texture) {
            ImGui::Image(m_texture->GetDescriptorSet(),
                            ImVec2(static_cast<float>(size.x), static_cast<float>(size.y)),
                            ImVec2(uv0.x, uv0.y),
                            ImVec2(uv1.x, uv1.y));
        }
    }

    std::unique_ptr<Image> Image::Load(SurfaceContext& context, std::filesystem::path const& path) {
        if (auto texture = Texture::FromFile(context, path)) {
            int const w = texture->GetWidth();
            int const h = texture->GetHeight();
            return std::make_unique<Image>(std::move(texture), IntRect{ { 0, 0 }, { w, h } });
        }
        return nullptr;
    }
}
