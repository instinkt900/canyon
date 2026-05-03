#pragma once

#include "moth_graphics/graphics/texture_filter.h"
#include "moth_graphics/graphics/texture_address_mode.h"
#include "moth_graphics/utils/rect.h"
#include "moth_graphics/utils/vector.h"

#include <filesystem>

namespace moth_graphics::graphics {
    /// @brief Abstract GPU texture resource.
    ///
    /// Owns raw pixel data on the GPU. Wrap in an @c Image to use with draw
    /// calls, or obtain one via @c AssetContext::TextureFromFile().
    class ITexture {
    public:
        virtual ~ITexture() = default;

        /// @brief Returns the width of the texture in pixels.
        virtual int GetWidth() const = 0;

        /// @brief Returns the height of the texture in pixels.
        virtual int GetHeight() const = 0;

        /// @brief Set the minification and magnification filters.
        /// @param minFilter Filter applied when the texture is scaled down.
        /// @param magFilter Filter applied when the texture is scaled up.
        virtual void SetFilter(TextureFilter minFilter, TextureFilter magFilter) = 0;

        /// @brief Set the UV address (wrap) mode for each axis.
        /// @param u Address mode along the horizontal axis.
        /// @param v Address mode along the vertical axis.
        virtual void SetAddressMode(TextureAddressMode u, TextureAddressMode v) = 0;

        /// @brief Render the texture as an ImGui image widget.
        /// @param size Display size in ImGui pixels.
        /// @param uv0 Top-left UV coordinate (texture-space, 0..1).
        /// @param uv1 Bottom-right UV coordinate (texture-space, 0..1).
        virtual void DrawImGui(IntVec2 const& size, FloatVec2 const& uv0, FloatVec2 const& uv1) const = 0;

        /// @brief Save a sub-region of the texture to a PNG file.
        /// @param path Destination file path.
        /// @param sourceRect Sub-region to save, in texture-space pixels.
        ///        @c sourceRect.w() and @c sourceRect.h() determine the output
        ///        image dimensions; @c sourceRect.topLeft is the offset within
        ///        the texture to start reading from.
        virtual void SaveToPNG(std::filesystem::path const& path, IntRect const& sourceRect) = 0;
    };
}
