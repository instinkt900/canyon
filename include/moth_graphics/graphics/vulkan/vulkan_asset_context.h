#pragma once

#include "moth_graphics/graphics/asset_context.h"
#include "moth_graphics/graphics/font_factory.h"
#include "moth_graphics/graphics/image_factory.h"
#include "moth_graphics/graphics/ifont.h"
#include "moth_graphics/graphics/iimage.h"
#include "moth_graphics/graphics/itexture.h"
#include "moth_graphics/utils/rect.h"

#include <filesystem>
#include <memory>

namespace moth_graphics::graphics::vulkan {
    class SurfaceContext;

    class AssetContext : public graphics::AssetContext {
    public:
        explicit AssetContext(SurfaceContext& context);
        ~AssetContext() override = default;

        AssetContext(AssetContext const&) = delete;
        AssetContext& operator=(AssetContext const&) = delete;
        AssetContext(AssetContext&&) = delete;
        AssetContext& operator=(AssetContext&&) = delete;

        graphics::ImageFactory& GetImageFactory() override { return m_imageFactory; }
        graphics::FontFactory& GetFontFactory() override { return m_fontFactory; }

        std::unique_ptr<IImage> NewImage(std::shared_ptr<ITexture> texture) override;
        std::unique_ptr<IImage> NewImage(std::shared_ptr<ITexture> texture, IntRect const& sourceRect) override;
        std::unique_ptr<IFont> FontFromFile(std::filesystem::path const& path, uint32_t size) override;
        std::unique_ptr<ITexture> TextureFromFile(std::filesystem::path const& path) override;
        std::unique_ptr<IImage> ImageFromFile(std::filesystem::path const& path) override;
        std::unique_ptr<ITexture> TextureFromPixels(int width, int height, uint8_t const* pixels) override;

    private:
        SurfaceContext& m_context;
        graphics::ImageFactory m_imageFactory;
        graphics::FontFactory m_fontFactory;
    };
}
