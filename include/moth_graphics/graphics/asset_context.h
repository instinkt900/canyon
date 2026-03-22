#pragma once

#include "moth_graphics/graphics/ifont.h"
#include "moth_graphics/graphics/iimage.h"
#include "moth_graphics/graphics/itexture.h"
#include "moth_graphics/utils/rect.h"

#include <memory>
#include <filesystem>
#include <cstdint>

namespace moth_graphics::graphics {
    /// @brief Per-window asset loading interface.
    ///
    /// Obtained from SurfaceContext::GetAssetContext(). Provides factory methods
    /// for loading GPU-backed assets (textures, images, fonts) that are bound to
    /// a specific window surface. Separates asset I/O concerns from the GPU
    /// resource management that SurfaceContext owns.
    class AssetContext {
    public:
        virtual ~AssetContext() = default;

        /// @brief Wrap an existing texture in a new image covering the full texture.
        virtual std::unique_ptr<IImage> NewImage(std::shared_ptr<ITexture> texture) = 0;

        /// @brief Wrap an existing texture in a new image covering a sub-region.
        virtual std::unique_ptr<IImage> NewImage(std::shared_ptr<ITexture> texture, IntRect const& sourceRect) = 0;

        /// @brief Load a font from a file at a given point size.
        virtual std::unique_ptr<IFont> FontFromFile(std::filesystem::path const& path, uint32_t size) = 0;

        /// @brief Load a texture from an image file.
        /// @returns Loaded texture, or @c nullptr on failure.
        virtual std::unique_ptr<ITexture> TextureFromFile(std::filesystem::path const& path) = 0;

        /// @brief Load an image directly from a file, combining texture load and image wrap.
        /// @returns Loaded image, or @c nullptr on failure.
        virtual std::unique_ptr<IImage> ImageFromFile(std::filesystem::path const& path) = 0;
    };
}
