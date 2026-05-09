#pragma once

#include "moth_graphics/graphics/spritesheet.h"
#include "moth_graphics/utils/rect.h"

#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace moth_graphics::graphics {
    class IGraphics;

    /// @brief Animated sprite driven by a SpriteSheet.
    ///
    /// Manages clip selection, frame advancement, and loop behaviour independently
    /// of any rendering layer. Call Update() each frame with the elapsed time in
    /// milliseconds, then use GetCurrentFrameRect() and GetImage() to render.
    ///
    /// Construct with a sprite sheet; SpriteSheet already enforces non-empty
    /// frames, so a successfully loaded sheet is always valid.
    class Sprite {
    public:
        explicit Sprite(std::shared_ptr<SpriteSheet> spriteSheet);

        /// @brief Returns the sprite sheet backing this sprite.
        SpriteSheet const& GetSpriteSheet() const { return *m_spriteSheet; }

        /// @brief Select a named clip and reset its playback position to the first step.
        ///
        /// This method selects the clip and resets the sequence position. When a
        /// valid clip is found the playing state is not changed — if the sprite was
        /// already playing it will continue into the new clip. If the name is empty
        /// or not found, the clip is cleared and playback is stopped (@c m_playing
        /// is set to @c false).
        /// @param name Clip name as defined in the descriptor, or empty to clear.
        void SetClip(std::string_view name);

        /// @brief Pause or resume playback of the current clip.
        /// @param playing @c true to resume, @c false to pause.
        void SetPlaying(bool playing);

        /// @brief Advance the animation by @p ticks milliseconds.
        void Update(uint32_t ticks);

        /// @brief Returns @c true if the current clip is actively advancing.
        bool IsPlaying() const { return m_playing; }

        /// @brief Returns the name of the currently active clip, or empty if none.
        std::string_view GetCurrentClipName() const { return m_currentClipName; }

        /// @brief Returns the atlas frame index currently being displayed.
        ///
        /// When a clip is active this is the frame index from the current clip step.
        /// When no clip is active (after SetFrame()) this is the raw atlas frame index.
        int GetCurrentFrame() const;

        /// @brief Display a specific atlas frame, stopping any active clip playback.
        /// @param frame Atlas frame index. Clamped to [0, GetFrameCount()).
        void SetFrame(int frame);

        /// @brief Returns the source rect of the current atlas frame within the sheet image.
        IntRect GetCurrentFrameRect() const;

        /// @brief Returns the pivot of the current atlas frame in pixels, relative to the frame's top-left corner.
        IntVec2 GetCurrentFramePivot() const;

        /// @brief Returns the width of the current frame in pixels.
        int GetWidth() const;

        /// @brief Returns the height of the current frame in pixels.
        int GetHeight() const;

        /// @brief Returns the underlying sprite sheet image.
        Image const& GetImage() const;

    private:
        std::shared_ptr<SpriteSheet> m_spriteSheet;
        std::optional<SpriteSheet::ClipDesc> m_currentClip;
        std::string m_currentClipName;
        int m_currentFrame = 0;    ///< Clip-sequence position when clip is active; raw atlas index otherwise.
        float m_accumulatedMs = 0.0f;
        bool m_playing = false;
    };

    /// @brief Draw the current frame of a sprite into a destination rectangle.
    void DrawSprite(IGraphics& graphics, Sprite const& sprite, IntRect const& destRect);

    /// @brief Draw the current frame of a sprite at a position, offset by a normalized pivot.
    void DrawSprite(IGraphics& graphics, Sprite const& sprite, IntVec2 const& pos, FloatVec2 const& pivot);

    /// @brief Draw the current frame of a sprite at a position using the frame's own pivot.
    void DrawSprite(IGraphics& graphics, Sprite const& sprite, IntVec2 const& pos);
}
