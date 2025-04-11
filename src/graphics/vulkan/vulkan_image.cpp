#include "common.h"
#include "canyon/graphics/vulkan/vulkan_image.h"
#include "canyon/graphics/vulkan/vulkan_graphics.h"

namespace canyon::graphics::vulkan {
    Graphics* Image::s_graphicsContext = nullptr;

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

    std::unique_ptr<Image> Image::Load(SurfaceContext& context, std::filesystem::path const& path) {
        if (auto texture = Texture::FromFile(context, path)) {
            return std::make_unique<Image>(std::move(texture), IntRect{ { 0, 0 }, { texture->GetWidth(), texture->GetHeight() }});
        }
        return nullptr;
    }
}
