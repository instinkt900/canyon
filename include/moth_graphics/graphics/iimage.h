#pragma once

#include "moth_graphics/graphics/itexture.h"
#include "moth_graphics/utils/vector.h"

namespace moth_graphics::graphics {
    /// @brief Abstract handle to a 2D image backed by a GPU texture.
    ///
    /// An image describes a rectangular region of an @c ITexture and exposes
    /// pixel dimensions for use by drawing and layout code.
    class IImage {
    public:
        virtual ~IImage() = default;

        /// @brief Returns the width of the image region in pixels.
        virtual int GetWidth() const = 0;

        /// @brief Returns the height of the image region in pixels.
        virtual int GetHeight() const = 0;

        /// @brief Returns the underlying GPU texture.
        virtual std::shared_ptr<ITexture> GetTexture() const = 0;

        /// @brief Render the image as an ImGui image widget.
        /// @param size Display size in ImGui pixels.
        /// @param uv0 Top-left UV coordinate (default: top-left of texture).
        /// @param uv1 Bottom-right UV coordinate (default: bottom-right of texture).
        virtual void ImGui(moth_graphics::IntVec2 const& size, moth_graphics::FloatVec2 const& uv0 = { 0, 0 }, moth_graphics::FloatVec2 const& uv1 = { 1, 1 }) const = 0;
    };
}
