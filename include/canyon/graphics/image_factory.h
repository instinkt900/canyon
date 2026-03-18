#pragma once

#include "canyon/graphics/asset_context.h"
#include "canyon/graphics/iimage.h"
#include "canyon/graphics/itexture.h"
#include "canyon/utils/rect.h"

#include <unordered_map>
#include <string>
#include <memory>
#include <filesystem>

namespace canyon::graphics {
    /// @brief Cached image loader.
    ///
    /// Loads images from disk on first request and returns cached copies on
    /// subsequent requests with the same path. Supports texture packs (atlases)
    /// in addition to individual image files.
    class ImageFactory {
    public:
        /// @param context The surface context used to create GPU resources.
        ImageFactory(AssetContext& context);
        virtual ~ImageFactory() = default;

        /// @brief Release all cached textures and images.
        void FlushCache();

        /// @brief Load a texture pack (atlas) from a JSON descriptor file.
        /// @param path Path to the texture pack descriptor.
        /// @returns @c true if the pack was loaded successfully.
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
