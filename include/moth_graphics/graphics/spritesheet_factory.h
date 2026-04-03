#pragma once

#include "moth_graphics/graphics/asset_context.h"
#include "moth_graphics/graphics/spritesheet.h"

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

namespace moth_graphics::graphics {
    /// @brief Cached sprite sheet loader.
    ///
    /// Parses .flipbook.json descriptors produced by moth_packer and loads the
    /// referenced sprite sheet image via the supplied AssetContext. Loaded sprite
    /// sheets are cached by canonical path and reused on subsequent requests.
    class SpriteSheetFactory {
    public:
        /// @param context The asset context used to load the sprite sheet image.
        explicit SpriteSheetFactory(AssetContext& context);
        virtual ~SpriteSheetFactory() = default;

        /// @brief Release all cached sprite sheets.
        void FlushCache();

        /// @brief Load or retrieve a cached sprite sheet by descriptor path.
        /// @param path Path to the .flipbook.json descriptor file.
        /// @return Loaded sprite sheet, or @c nullptr on failure.
        std::shared_ptr<SpriteSheet> GetSpriteSheet(std::filesystem::path const& path);

    private:
        AssetContext& m_context;
        std::unordered_map<std::string, std::shared_ptr<SpriteSheet>> m_cache;
    };
}
