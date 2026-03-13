#pragma once

#include "canyon/graphics/blend_mode.h"
#include "canyon/graphics/color.h"
#include "canyon/graphics/ifont.h"
#include "canyon/graphics/iimage.h"
#include "canyon/graphics/itarget.h"
#include "canyon/graphics/text_alignment.h"
#include "canyon/utils/rect.h"
#include "canyon/utils/vector.h"

#include <filesystem>
#include <memory>

namespace canyon::platform {
    class Window;
}

namespace canyon::graphics {
    class SurfaceContext;

    /// @brief Abstract 2D rendering interface.
    ///
    /// All drawing operations are batched between a @c Begin() / @c End() pair.
    /// State (colour, blend mode, clip rect, render target) is set before issuing
    /// draw calls and remains active until changed.
    class IGraphics {
    public:
        virtual ~IGraphics() {}

        /// @brief Initialise the ImGui backend for the given window.
        /// @param window The window whose native handle ImGui will attach to.
        virtual void InitImgui(canyon::platform::Window const& window) = 0;

        /// @brief Returns the surface context that owns this graphics instance.
        virtual SurfaceContext& GetContext() const = 0;

        /// @brief Begin a new frame. Must be called before any draw operations.
        virtual void Begin() = 0;

        /// @brief End the current frame and present it.
        virtual void End() = 0;

        /// @brief Set the active blend mode for subsequent draw calls.
        /// @param mode The blend mode to apply.
        virtual void SetBlendMode(BlendMode mode) = 0;

        /// @brief Set the active draw colour for subsequent draw calls.
        /// @param color The colour to use (also modulates image draws).
        virtual void SetColor(Color const& color) = 0;

        /// @brief Fill the entire render target with the current colour.
        virtual void Clear() = 0;

        /// @brief Draw an image into a destination rectangle.
        /// @param image The image to draw.
        /// @param destRect Destination rectangle in logical pixels.
        /// @param sourceRect Sub-region of the image to sample, or @c nullptr for the full image.
        /// @param rotation Clockwise rotation in degrees about the centre of @p destRect.
        virtual void DrawImage(IImage& image, IntRect const& destRect, IntRect const* sourceRect = nullptr, float rotation = 0.0f) = 0;

        /// @brief Tile an image to fill a destination rectangle.
        /// @param image The image to tile.
        /// @param destRect Destination rectangle in logical pixels.
        /// @param sourceRect Sub-region of the image to tile, or @c nullptr for the full image.
        /// @param scale Scale factor applied to each tile.
        virtual void DrawImageTiled(IImage& image, IntRect const& destRect, IntRect const* sourceRect = nullptr, float scale = 1.0f) = 0;

        /// @brief Capture the current render target and write it to a PNG file.
        /// @param path Destination file path.
        virtual void DrawToPNG(std::filesystem::path const& path) = 0;

        /// @brief Draw an axis-aligned rectangle outline using the current colour.
        /// @param rect Rectangle in logical pixels.
        virtual void DrawRectF(FloatRect const& rect) = 0;

        /// @brief Draw a filled axis-aligned rectangle using the current colour.
        /// @param rect Rectangle in logical pixels.
        virtual void DrawFillRectF(FloatRect const& rect) = 0;

        /// @brief Draw a line segment using the current colour.
        /// @param p0 Start point in logical pixels.
        /// @param p1 End point in logical pixels.
        virtual void DrawLineF(FloatVec2 const& p0, FloatVec2 const& p1) = 0;

        /// @brief Draw a string of text into a destination rectangle.
        /// @param text UTF-8 text to render.
        /// @param font Font to use for rendering.
        /// @param destRect Bounding rectangle in logical pixels.
        /// @param horizontalAlignment Horizontal alignment within @p destRect.
        /// @param verticalAlignment Vertical alignment within @p destRect.
        virtual void DrawText(std::string const& text, IFont& font, IntRect const& destRect, TextHorizAlignment horizontalAlignment = TextHorizAlignment::Left, TextVertAlignment verticalAlignment = TextVertAlignment::Top) = 0;

        /// @brief Set the scissor clip rectangle. Pass @c nullptr to clear clipping.
        /// @param rect Clip rectangle in logical pixels, or @c nullptr to disable.
        virtual void SetClip(IntRect const* rect) = 0;

        /// @brief Create an off-screen render target.
        /// @param width Width in pixels.
        /// @param height Height in pixels.
        /// @returns Ownership of a new render target.
        virtual std::unique_ptr<ITarget> CreateTarget(int width, int height) = 0;

        /// @brief Returns the currently active render target, or @c nullptr if rendering to the swapchain.
        virtual ITarget* GetTarget() = 0;

        /// @brief Set the active render target. Pass @c nullptr to restore rendering to the swapchain.
        /// @param target Render target to draw into, or @c nullptr.
        virtual void SetTarget(ITarget* target) = 0;

        /// @brief Override the logical rendering resolution used to map draw coordinates.
        /// @param logicalSize Logical width and height in pixels.
        virtual void SetLogicalSize(IntVec2 const& logicalSize) = 0;
    };
}
