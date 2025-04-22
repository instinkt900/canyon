#include "common.h"
#include "canyon/graphics/vulkan/vulkan_image.h"
#include "canyon/graphics/vulkan/vulkan_graphics.h"

namespace canyon::graphics::vulkan {
    Image::Image(std::shared_ptr<Texture> texture)
    : m_texture(texture) 
    , m_sourceRect({{ 0, 0 }, { texture->GetWidth(), texture->GetHeight() }}) {

    }

    Image::Image(std::shared_ptr<Texture> texture, IntRect const& sourceRect)
        : m_texture(texture)
        , m_sourceRect(sourceRect) {
    }

    Image::~Image() {
    }

    int Image::GetWidth() const {
        return m_sourceRect.bottomRight.x - m_sourceRect.topLeft.x;
    }

    int Image::GetHeight() const {
        return m_sourceRect.bottomRight.y - m_sourceRect.topLeft.y;
    }


    void Image::ImGui(canyon::IntVec2 const& size, canyon::FloatVec2 const& uv0, canyon::FloatVec2 const& uv1) const {
        if (m_texture) {
            ImGui::Image(m_texture->GetDescriptorSet(),
                            ImVec2(static_cast<float>(size.x), static_cast<float>(size.y)),
                            ImVec2(uv0.x, uv0.y),
                            ImVec2(uv1.x, uv1.y));
        }
    }

    std::unique_ptr<Image> Image::Load(SurfaceContext& context, std::filesystem::path const& path) {
        if (auto texture = Texture::FromFile(context, path)) {
            return std::make_unique<Image>(std::move(texture), IntRect{ { 0, 0 }, { texture->GetWidth(), texture->GetHeight() }});
        }
        return nullptr;
    }
}
