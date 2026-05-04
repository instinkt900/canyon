#pragma once

#include "moth_graphics/graphics/blend_mode.h"
#include "moth_graphics/graphics/color.h"
#include "moth_graphics/graphics/ifont.h"
#include "moth_graphics/graphics/iimage.h"
#include "moth_graphics/graphics/itarget.h"
#include "moth_graphics/graphics/text_alignment.h"
#include "moth_graphics/utils/rect.h"
#include "moth_graphics/utils/transform.h"
#include "moth_graphics/utils/vector.h"

#include <memory>

namespace moth_graphics::platform {
    class Window;
}

namespace moth_graphics::graphics {
    class SurfaceContext;

    /// @brief Abstract 2D rendering interface.
    ///
    /// All drawing operations are batched between a @c Begin() / @c End() pair.
    /// State (color, blend mode, clip rect, render target) is set before issuing
    /// draw calls and remains active until changed.
    class IGraphics {
    public:
        virtual ~IGraphics() {}

        /// @brief Initialize the ImGui backend for the given window.
        /// @param window The window whose native handle ImGui will attach to.
        /// @param enableViewports If true, enables ImGui multi-viewport support (promotes
        ///        floating windows to native OS windows). Not supported by the SDL2 renderer
        ///        backend; ignored there. Avoid on tiling window managers (i3, sway) where
        ///        new OS windows disrupt drag-and-drop.
        virtual void InitImgui(moth_graphics::platform::Window const& window, bool enableViewports = false) = 0;

        /// @brief Returns the surface context that owns this graphics instance.
        virtual SurfaceContext& GetSurfaceContext() const = 0;

        /// @brief Begin a new frame. Must be called before any draw operations.
        /// @returns @c true if the frame can proceed, @c false if the swapchain was
        ///          out-of-date and the frame should be skipped (e.g. the window is
        ///          minimised or being resized). When @c false is returned, @c End()
        ///          must NOT be called for this frame.
        virtual bool Begin() = 0;

        /// @brief End the current frame and present it. Only call when @c Begin()
        ///        returned @c true.
        virtual void End() = 0;

        /// @brief Set the active blend mode for subsequent draw calls.
        /// @param mode The blend mode to apply.
        virtual void SetBlendMode(BlendMode mode) = 0;

        /// @brief Set the active draw color for subsequent draw calls.
        /// @param color The color to use (also modulates image draws).
        virtual void SetColor(Color const& color) = 0;

        /// @brief Fill the entire render target with the current color.
        virtual void Clear() = 0;

        /// @brief Set the active transform applied to all subsequent draw coordinates.
        /// @param transform Local-to-world transform for subsequent draw calls.
        virtual void SetTransform(FloatMat4x4 const& transform) = 0;

        /// @brief Draw an image into a destination rectangle in local space. The active transform is applied.
        /// @param image The image to draw.
        /// @param destRect Destination rectangle in local (pre-transform) space.
        /// @param sourceRect Sub-region of the image to sample, or @c nullptr for the full image.
        virtual void DrawImage(Image const& image, IntRect const& destRect, IntRect const* sourceRect = nullptr) = 0;

        /// @brief Draw an image at a position, offset so that @p pivot within the image aligns with @p pos.
        /// @param image  The image to draw at natural size.
        /// @param pos    Destination point in logical pixels.
        /// @param pivot  Normalized pivot within the image: {0,0} = top-left, {0.5,0.5} = center,
        ///               {1,1} = bottom-right. Defaults to center.
        virtual void DrawImage(Image const& image, IntVec2 const& pos, FloatVec2 const& pivot = { 0.5f, 0.5f }) = 0;

        /// @brief Tile an image to fill a destination rectangle.
        /// @param image The image to tile.
        /// @param destRect Destination rectangle in logical pixels.
        /// @param sourceRect Sub-region of the image to tile, or @c nullptr for the full image.
        /// @param scale Scale factor applied to each tile.
        virtual void DrawImageTiled(Image const& image, IntRect const& destRect, IntRect const* sourceRect = nullptr, float scale = 1.0f) = 0;

        /// @brief Draw an axis-aligned rectangle outline using the current color.
        /// @param rect Rectangle in logical pixels.
        virtual void DrawRectF(FloatRect const& rect) = 0;

        /// @brief Draw a filled axis-aligned rectangle using the current color.
        /// @param rect Rectangle in logical pixels.
        virtual void DrawFillRectF(FloatRect const& rect) = 0;

        /// @brief Draw a line segment using the current color.
        /// @param p0 Start point in logical pixels.
        /// @param p1 End point in logical pixels.
        virtual void DrawLineF(FloatVec2 const& p0, FloatVec2 const& p1) = 0;

        /// @brief Draw a string of text into a destination rectangle.
        /// @param text UTF-8 text to render.
        /// @param font Font to use for rendering.
        /// @param destRect Bounding rectangle in logical pixels.
        /// @param horizontalAlignment Horizontal alignment within @p destRect.
        /// @param verticalAlignment Vertical alignment within @p destRect.
        virtual void DrawText(std::string_view text, IFont& font, IntRect const& destRect, TextHorizAlignment horizontalAlignment = TextHorizAlignment::Left, TextVertAlignment verticalAlignment = TextVertAlignment::Top) = 0;

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
