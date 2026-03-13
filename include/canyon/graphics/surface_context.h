#pragma once

#include "canyon/graphics/context.h"
#include "canyon/graphics/ifont.h"
#include "canyon/graphics/iimage.h"
#include "canyon/graphics/itexture.h"
#include "canyon/utils/rect.h"

#include <memory>
#include <filesystem>
#include <cstdint>

namespace canyon::graphics {
    /// @brief Per-window graphics resource context.
    ///
    /// Owns the GPU device resources associated with a single window surface
    /// (command pools, descriptor pools, allocators, etc.) and acts as the
    /// factory for all GPU-backed assets (textures, images, fonts) that belong
    /// to that window.
    class SurfaceContext {
    public:
        virtual ~SurfaceContext() = default;

        /// @brief Returns the top-level graphics context (Vulkan instance, etc.).
        virtual Context& GetContext() const = 0;

        /// @brief Wrap an existing texture in a new image covering the full texture.
        /// @param texture Texture to wrap.
        virtual std::unique_ptr<IImage> NewImage(std::shared_ptr<ITexture> texture) = 0;

        /// @brief Wrap an existing texture in a new image covering a sub-region.
        /// @param texture Texture to wrap.
        /// @param sourceRect Pixel rectangle within @p texture to expose.
        virtual std::unique_ptr<IImage> NewImage(std::shared_ptr<ITexture> texture, IntRect const& sourceRect) = 0;

        /// @brief Load a font from a file at a given point size.
        /// @param path Path to the font file (TTF/OTF).
        /// @param size Point size to rasterise at.
        virtual std::unique_ptr<IFont> FontFromFile(std::filesystem::path const& path, uint32_t size) = 0;

        /// @brief Load a texture from an image file.
        /// @param path Path to the image file (PNG, JPEG, etc.).
        /// @returns Loaded texture, or @c nullptr on failure.
        virtual std::unique_ptr<ITexture> TextureFromFile(std::filesystem::path const& path) = 0;

        /// @brief Load an image directly from a file, combining texture load and image wrap.
        /// @param path Path to the image file.
        /// @returns Loaded image, or @c nullptr on failure.
        virtual std::unique_ptr<IImage> ImageFromFile(std::filesystem::path const& path) = 0;
    };
}
