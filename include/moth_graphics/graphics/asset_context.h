#pragma once

#include "moth_graphics/graphics/ifont.h"
#include "moth_graphics/graphics/image.h"
#include "moth_graphics/graphics/itexture.h"
#include "moth_graphics/utils/rect.h"

#include <memory>
#include <filesystem>
#include <cstdint>

namespace moth_graphics::graphics {
    class FontFactory;
    class SpriteSheetFactory;
    class TextureFactory;

    /// @brief Per-window asset loading interface.
    ///
    /// Obtained from SurfaceContext::GetAssetContext(). Provides factory methods
    /// for loading GPU-backed assets (textures, fonts, sprite sheets) that are
    /// bound to a specific window surface. Separates asset I/O concerns from the
    /// GPU resource management that SurfaceContext owns.
    class AssetContext {
    public:
        virtual ~AssetContext() = default;

        /// @brief Returns the cached texture factory for this context.
        virtual TextureFactory& GetTextureFactory() = 0;

        /// @brief Returns the cached font factory for this context.
        virtual FontFactory& GetFontFactory() = 0;

        /// @brief Returns the cached sprite sheet factory for this context.
        virtual SpriteSheetFactory& GetSpriteSheetFactory() = 0;

        /// @brief Load a font from a file at a given pixel size.
        virtual std::unique_ptr<IFont> FontFromFile(std::filesystem::path const& path, uint32_t size) = 0;

        /// @brief Load a texture from an image file.
        /// @returns Loaded texture, or @c nullptr on failure.
        virtual std::unique_ptr<ITexture> TextureFromFile(std::filesystem::path const& path) = 0;

        /// @brief Create a texture from raw RGBA pixel data (4 bytes per pixel: R, G, B, A).
        /// @param width Texture width in pixels.
        /// @param height Texture height in pixels.
        /// @param pixels Pointer to @c width * height * 4 bytes of RGBA data.
        /// @returns Loaded texture, or @c nullptr on failure.
        virtual std::unique_ptr<ITexture> TextureFromPixels(int width, int height, uint8_t const* pixels) = 0;
    };
}
