#pragma once

#include "moth_graphics/graphics/iimage.h"

#include <memory>
#include <string_view>

namespace moth_graphics::graphics {
    /// @brief Abstract representation of a loaded sprite sheet animation.
    ///
    /// Exposes the underlying sprite sheet image and the uniform-grid layout and
    /// named clip data needed to drive frame animation. Backend implementations
    /// are agnostic to the moth_ui playback layer.
    class ISpriteSheet {
    public:
        /// @brief Controls what happens when a clip reaches its last frame.
        enum class LoopType {
            Stop,   ///< Freeze on the last frame and signal completion.
            Reset,  ///< Rewind to the first frame, freeze, and signal completion.
            Loop,   ///< Jump back to the first frame and continue playing.
        };

        /// @brief Describes the uniform grid layout of the sprite sheet.
        struct SheetDesc {
            IntVec2 FrameDimensions; ///< Width and height of a single frame in pixels.
            IntVec2 SheetCells;      ///< Grid size: x = number of columns, y = number of rows.
            int MaxFrames = 0;       ///< Total number of usable frames (<= cols * rows).
            int NumClips  = 0;       ///< Number of named clips defined on this sprite sheet.
        };

        /// @brief Describes a named animation range within the sprite sheet.
        struct ClipDesc {
            int Start = 0;                  ///< Index of the first frame (inclusive).
            int End   = 0;                  ///< Index of the last frame (inclusive).
            int FPS   = 0;                  ///< Playback rate in frames per second.
            LoopType Loop = LoopType::Stop; ///< Playback behaviour when the end is reached.
        };

        /// @brief Returns the sprite sheet image used for all frames.
        virtual std::shared_ptr<IImage> GetImage() const = 0;

        /// @brief Populates @p outDesc with the sheet geometry and clip count.
        virtual void GetSheetDesc(SheetDesc& outDesc) const = 0;

        /// @brief Returns the name of the clip at the given index.
        /// @return Clip name, or an empty string_view if the index is out of range.
        virtual std::string_view GetClipName(int index) const = 0;

        /// @brief Looks up a clip by name and populates its description.
        /// @return @c true if the clip was found, @c false otherwise.
        virtual bool GetClipDesc(std::string_view name, ClipDesc& outDesc) const = 0;

        ISpriteSheet() = default;
        ISpriteSheet(ISpriteSheet const&) = default;
        ISpriteSheet(ISpriteSheet&&) = default;
        ISpriteSheet& operator=(ISpriteSheet const&) = default;
        ISpriteSheet& operator=(ISpriteSheet&&) = default;
        virtual ~ISpriteSheet() = default;
    };
}
