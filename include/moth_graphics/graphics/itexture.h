#pragma once

#include "moth_graphics/graphics/texture_filter.h"
#include "moth_graphics/graphics/texture_address_mode.h"

namespace moth_graphics::graphics {
    /// @brief Abstract GPU texture resource.
    ///
    /// Owns raw pixel data on the GPU. Wrap in an @c IImage to use with draw
    /// calls, or obtain one via @c SurfaceContext::TextureFromFile().
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
    };
}
