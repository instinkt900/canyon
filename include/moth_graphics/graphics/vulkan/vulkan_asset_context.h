#pragma once

#include "moth_graphics/graphics/asset_context.h"
#include "moth_graphics/graphics/font_factory.h"
#include "moth_graphics/graphics/texture_factory.h"
#include "moth_graphics/graphics/spritesheet_factory.h"
#include "moth_graphics/graphics/ifont.h"
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

        graphics::TextureFactory& GetTextureFactory() override { return m_textureFactory; }
        graphics::FontFactory& GetFontFactory() override { return m_fontFactory; }
        graphics::SpriteSheetFactory& GetSpriteSheetFactory() override { return m_spriteSheetFactory; }

        std::unique_ptr<IFont> FontFromFile(std::filesystem::path const& path, uint32_t size) override;
        std::unique_ptr<ITexture> TextureFromFile(std::filesystem::path const& path) override;
        std::unique_ptr<ITexture> TextureFromPixels(int width, int height, uint8_t const* pixels) override;

    private:
        SurfaceContext& m_context;
        graphics::TextureFactory m_textureFactory;
        graphics::FontFactory m_fontFactory;
        graphics::SpriteSheetFactory m_spriteSheetFactory;
    };
}
