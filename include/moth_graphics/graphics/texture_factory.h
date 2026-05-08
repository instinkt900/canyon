#pragma once

#include "moth_graphics/graphics/asset_context.h"
#include "moth_graphics/graphics/itexture.h"
#include "moth_graphics/utils/rect.h"

#include <unordered_map>
#include <optional>
#include <string>
#include <memory>
#include <filesystem>

namespace moth_graphics::graphics {
    /// @brief Cached texture loader.
    ///
    /// Loads textures from disk on first request and returns cached copies on
    /// subsequent requests with the same path. Supports texture packs (atlases)
    /// in addition to individual texture files.
    ///
    /// To construct an @c Image from a loaded texture:
    /// @code
    ///   auto texture = factory.GetTexture(path);
    ///   if (texture) {
    ///       Image image{ texture, factory.GetTextureRect(path) };
    ///   }
    /// @endcode
    class TextureFactory {
    public:
        /// @param context The asset context used to load texture assets.
        TextureFactory(AssetContext& context);
        virtual ~TextureFactory() = default;

        /// @brief Release all cached textures.
        void FlushCache();

        /// @brief Load a texture pack from a JSON descriptor file.
        ///
        /// The descriptor lists one or more atlas PNGs and the sub-images packed
        /// into each one. All referenced PNGs are loaded and their sub-regions
        /// registered in the cache. See moth_packer for the descriptor format.
        /// @param path Path to the pack descriptor JSON (e.g. @c ui.json).
        /// @returns @c true if at least the descriptor was parsed successfully.
        bool LoadTexturePack(std::filesystem::path const& path);

        /// @brief Load or retrieve a cached texture by file path.
        ///
        /// If a texture pack has been loaded and contains the given path, the
        /// atlas texture is returned. Otherwise the file is loaded directly.
        /// Returns the fallback texture if the path cannot be loaded and one has
        /// been set.
        /// @param path Path to the texture file or pack entry.
        /// @returns A shared texture, or @c nullptr on failure.
        std::shared_ptr<ITexture> GetTexture(std::filesystem::path const& path);

        /// @brief Returns the source rectangle for a cached path.
        ///
        /// For texture-pack entries this is the sub-region within the atlas
        /// texture. For standalone textures this is the full texture rect
        /// @c {0, 0, width, height}. Returns a default (zero) rect if the path
        /// has not been loaded.
        IntRect GetTextureRect(std::filesystem::path const& path) const;

        /// @brief Register a texture to return as a fallback when a texture cannot be loaded.
        /// @param texture Texture to use for the fallback; covers the full texture dimensions.
        void SetFallbackTexture(std::shared_ptr<ITexture> texture);
        void SetFallbackTexture(std::shared_ptr<ITexture> texture, IntRect const& sourceRect);

    private:
        struct TextureDesc {
            std::shared_ptr<ITexture> m_texture;
            IntRect m_sourceRect;
            std::filesystem::path m_path;
        };

        AssetContext& m_context;
        std::unordered_map<std::string, TextureDesc> m_cachedTextures;
        std::optional<TextureDesc> m_fallbackDesc;
    };
}
