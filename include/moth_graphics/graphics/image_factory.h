#pragma once

#include "moth_graphics/graphics/asset_context.h"
#include "moth_graphics/graphics/iimage.h"
#include "moth_graphics/graphics/itexture.h"
#include "moth_graphics/utils/rect.h"

#include <unordered_map>
#include <optional>
#include <string>
#include <memory>
#include <filesystem>

namespace moth_graphics::graphics {
    /// @brief Cached image loader.
    ///
    /// Loads images from disk on first request and returns cached copies on
    /// subsequent requests with the same path. Supports texture packs (atlases)
    /// in addition to individual image files.
    class ImageFactory {
    public:
        /// @param context The asset context used to load image assets.
        ImageFactory(AssetContext& context);
        virtual ~ImageFactory() = default;

        /// @brief Release all cached textures and images.
        void FlushCache();

        /// @brief Load a texture pack from a JSON descriptor file.
        ///
        /// The descriptor lists one or more atlas PNGs and the images packed into
        /// each one. All referenced PNGs are loaded and their images registered in
        /// the cache. See moth_packer for the descriptor format.
        /// @param path Path to the pack descriptor JSON (e.g. @c ui.json).
        /// @returns @c true if at least the descriptor was parsed successfully.
        bool LoadTexturePack(std::filesystem::path const& path);

        /// @brief Load or retrieve a cached image by file path.
        ///
        /// If a texture pack has been loaded and contains the given path, the
        /// image is sourced from the atlas. Otherwise the file is loaded directly.
        /// Returns the fallback image if the path cannot be loaded and one has been set.
        /// @param path Path to the image file.
        /// @returns A new image handle, or @c nullptr on failure.
        std::unique_ptr<IImage> GetImage(std::filesystem::path const& path);

        /// @brief Register a texture to return as a fallback when an image cannot be loaded.
        /// @param texture Texture to use for the fallback; covers the full texture dimensions.
        void SetFallbackImage(std::shared_ptr<ITexture> texture);

    private:
        struct ImageDesc {
            std::shared_ptr<ITexture> m_texture;
            IntRect sourceRect;
            std::filesystem::path m_path;
        };

        AssetContext& m_context;
        std::unordered_map<std::string, ImageDesc> m_cachedImages;
        std::optional<ImageDesc> m_fallbackDesc;
    };
}
