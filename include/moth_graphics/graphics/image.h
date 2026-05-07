#pragma once

#include "moth_graphics/graphics/itexture.h"
#include "moth_graphics/utils/rect.h"
#include "moth_graphics/utils/vector.h"

#include <filesystem>
#include <memory>

namespace moth_graphics::graphics {
    /// @brief A handle to a 2D image: a texture plus an optional sub-region rect.
    ///
    /// Images are cheap value types — copying one shares the underlying GPU
    /// texture via @c std::shared_ptr. Construct directly from a texture (with
    /// or without a sub-region), or obtain one from @c TextureFactory::GetTexture
    /// combined with @c GetTextureRect. Construct directly at the call site:
    /// @code
    ///   auto tex = factory.GetTexture(path);
    ///   if (tex) { Image image{ tex, factory.GetTextureRect(path) }; }
    /// @endcode
    ///
    /// A default-constructed @c Image is empty (@c operator @c bool returns
    /// @c false). Factories return an empty @c Image to indicate failure.
    class Image {
    public:
        /// @brief Construct an empty image (no texture).
        Image() = default;

        /// @brief Construct an image covering an entire texture.
        /// @param texture Backing texture. Must not be null for the image to be valid.
        explicit Image(std::shared_ptr<ITexture> texture);

        /// @brief Construct an image covering a sub-region of a texture.
        /// @param texture Backing texture. Must not be null for the image to be valid.
        /// @param sourceRect Sub-region of @p texture to display, in pixels.
        Image(std::shared_ptr<ITexture> texture, IntRect const& sourceRect);

        /// @brief Returns the width of the source rectangle in pixels.
        int GetWidth() const { return m_sourceRect.bottomRight.x - m_sourceRect.topLeft.x; }

        /// @brief Returns the height of the source rectangle in pixels.
        int GetHeight() const { return m_sourceRect.bottomRight.y - m_sourceRect.topLeft.y; }

        /// @brief Returns the underlying GPU texture (may be null for an empty image).
        std::shared_ptr<ITexture> const& GetTexture() const { return m_texture; }

        /// @brief Returns the source rectangle within the texture.
        IntRect const& GetSourceRect() const { return m_sourceRect; }

        /// @brief Returns @c true if this image has a backing texture.
        explicit operator bool() const { return m_texture != nullptr; }

        /// @brief Render the image's source rectangle as an ImGui image widget.
        /// @param size Display size in ImGui pixels.
        ///
        /// UVs are derived from @c m_sourceRect relative to the backing texture
        /// dimensions, so atlas / sub-region images render only their region.
        /// Equivalent to @c DrawImGui(size, {0,0}, {1,1}).
        void DrawImGui(IntVec2 const& size) const;

        /// @brief Render a sub-region of the image as an ImGui image widget.
        /// @param size Display size in ImGui pixels.
        /// @param uv0 Top-left UV coordinate relative to the image's source rect
        ///            (0,0 = top-left of source rect, 1,1 = bottom-right).
        /// @param uv1 Bottom-right UV coordinate relative to the image's source rect.
        ///
        /// The UVs are remapped through the source rectangle so callers can
        /// work in image-local coordinates regardless of whether the image is
        /// a standalone texture or an atlas sub-region.
        void DrawImGui(IntVec2 const& size, FloatVec2 const& uv0, FloatVec2 const& uv1) const;

        /// @brief Save this image's source region to a PNG file.
        /// @param path Destination file path.
        void SaveToPNG(std::filesystem::path const& path);

    private:
        std::shared_ptr<ITexture> m_texture;
        IntRect m_sourceRect;
    };
}
