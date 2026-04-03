#pragma once

#include "moth_graphics/graphics/iimage.h"

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace moth_graphics::graphics {
    /// @brief A loaded sprite sheet animation.
    ///
    /// Holds a sprite sheet image and its uniform-grid layout and named clip data.
    /// Construct directly for programmatic use, or load from a .flipbook.json
    /// descriptor via SpriteSheetFactory.
    class SpriteSheet {
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

        struct ClipEntry {
            std::string name;
            ClipDesc desc;
        };

        SpriteSheet(std::shared_ptr<IImage> image,
                    SheetDesc sheetDesc,
                    std::vector<ClipEntry> clips);

        std::shared_ptr<IImage> GetImage() const;
        void GetSheetDesc(SheetDesc& outDesc) const;
        std::string_view GetClipName(int index) const;
        bool GetClipDesc(std::string_view name, ClipDesc& outDesc) const;

    private:
        std::shared_ptr<IImage> m_image;
        SheetDesc m_sheetDesc;
        std::vector<ClipEntry> m_clips;
    };
}
