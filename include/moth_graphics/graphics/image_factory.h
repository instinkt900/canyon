#pragma once

#include "moth_graphics/graphics/asset_context.h"
#include "moth_graphics/graphics/iimage.h"
#include "moth_graphics/graphics/itexture.h"
#include "moth_graphics/utils/rect.h"

#include <unordered_map>
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
        /// @param path Path to the image file.
        /// @returns A new image handle, or @c nullptr on failure.
        std::unique_ptr<IImage> GetImage(std::filesystem::path const& path);

    private:
        struct ImageDesc {
            std::shared_ptr<ITexture> m_texture;
            IntRect m_sourceRect;
            std::filesystem::path m_path;
        };

        AssetContext& m_context;
        std::unordered_map<std::string, ImageDesc> m_cachedImages;
    };
}
