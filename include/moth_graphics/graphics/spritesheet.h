#pragma once

#include "moth_graphics/graphics/image.h"
#include "moth_graphics/utils/rect.h"

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace moth_graphics::graphics {
    /// @brief A loaded sprite sheet animation.
    ///
    /// Holds a sprite sheet image plus per-frame rects/pivots and named clip data.
    /// Frames may vary in size; clips are explicit ordered sequences of frame steps,
    /// each with its own display duration.
    ///
    /// Construct directly for programmatic use, or load from a .flipbook.json
    /// descriptor via SpriteSheetFactory.
    class SpriteSheet {
    public:
        /// @brief Controls what happens when a clip reaches its last step.
        enum class LoopType {
            Stop,   ///< Freeze on the last frame and signal completion.
            Reset,  ///< Rewind to the first frame, freeze, and signal completion.
            Loop,   ///< Jump back to the first frame and continue playing.
        };

        /// @brief Describes a single frame within the atlas.
        struct FrameEntry {
            IntRect rect;   ///< Source rect within the atlas image, in pixels.
            IntVec2 pivot;  ///< Pivot point relative to the frame's top-left corner, in pixels.
        };

        /// @brief A single step within a clip sequence.
        struct ClipFrame {
            int frameIndex = 0;  ///< Index into the sprite sheet's frame list.
            int durationMs = 0;  ///< How long this step is displayed, in milliseconds.
        };

        /// @brief Describes a named animation clip.
        struct ClipDesc {
            std::vector<ClipFrame> frames;      ///< Ordered sequence of steps.
            LoopType loop = LoopType::Stop;     ///< Playback behaviour when the last step is reached.
        };

        struct ClipEntry {
            std::string name;
            ClipDesc desc;
        };

        /// @brief Constructs a sprite sheet.
        /// @param image  Atlas image. Must not be null.
        /// @param frames Per-frame rects and pivots. Must not be empty; each frame's
        ///               width and height must be > 0.
        /// @param clips  Named animation clips. Each clip's @c frames list must be
        ///               non-empty, every @c frameIndex must be in [0, frames.size()),
        ///               and every @c durationMs must be > 0. These are the same
        ///               invariants enforced by SpriteSheetFactory when loading from
        ///               a descriptor file.
        SpriteSheet(Image image,
                    std::vector<FrameEntry> frames,
                    std::vector<ClipEntry> clips);

        Image const& GetImage() const;

        /// @brief Returns the total number of frames in the atlas.
        int GetFrameCount() const;

        /// @brief Returns the frame at @p index, or @c std::nullopt if out of range.
        std::optional<FrameEntry> GetFrameDesc(int index) const;

        /// @brief Returns the number of named clips.
        int GetClipCount() const;

        /// @brief Returns the clip name at @p index, or empty if out of range.
        std::string_view GetClipName(int index) const;

        /// @brief Looks up a clip by name, or @c std::nullopt if not found.
        std::optional<ClipDesc> GetClipDesc(std::string_view name) const;

    private:
        Image m_image;
        std::vector<FrameEntry> m_frames;
        std::vector<ClipEntry> m_clips;
    };
}
