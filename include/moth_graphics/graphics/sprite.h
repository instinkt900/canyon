#pragma once

#include "moth_graphics/graphics/spritesheet.h"
#include "moth_graphics/utils/rect.h"

#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace moth_graphics::graphics {
    /// @brief Animated sprite driven by a SpriteSheet.
    ///
    /// Manages clip selection, frame advancement, and loop behaviour independently
    /// of any rendering layer. Call Update() each frame with the elapsed time in
    /// milliseconds, then pass the Sprite to IGraphics::DrawSprite() to render the
    /// current frame.
    ///
    /// A Sprite must always have an associated sprite sheet. Use Create() to
    /// construct one; it returns @c nullptr if the sheet descriptor is invalid.
    class Sprite {
    public:
        /// @brief Attempt to create a Sprite from a sprite sheet.
        /// @param spriteSheet The sprite sheet to animate. Must not be null.
        /// @return A Sprite on success, or @c nullptr if the sheet descriptor is invalid.
        static std::unique_ptr<Sprite> Create(std::shared_ptr<SpriteSheet> spriteSheet);

        Sprite(Sprite&&) = default;
        Sprite& operator=(Sprite&&) = default;
        Sprite(Sprite const&) = default;
        Sprite& operator=(Sprite const&) = default;

        /// @brief Returns the sprite sheet backing this sprite.
        SpriteSheet const& GetSpriteSheet() const { return *m_spriteSheet; }

        /// @brief Activate a named clip and reset playback to its first frame.
        ///
        /// The sprite will not animate until a clip has been set. If the name is
        /// not found the current clip is cleared and playback stops.
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

        /// @brief Returns the current frame index within the full sheet grid.
        int GetCurrentFrame() const { return m_currentFrame; }

        /// @brief Manually set the current frame index, stopping any active clip playback.
        /// @param frame Frame index within the full sheet grid. Clamped to [0, MaxFrames).
        void SetFrame(int frame);

        /// @brief Returns the source rect of the current frame within the sheet image.
        IntRect GetCurrentFrameRect() const;

        /// @brief Returns the frame width in pixels.
        int GetWidth() const { return m_sheetDesc.FrameDimensions.x; }

        /// @brief Returns the frame height in pixels.
        int GetHeight() const { return m_sheetDesc.FrameDimensions.y; }

        /// @brief Returns the underlying sprite sheet image.
        IImage* GetImage() const;

    private:
        Sprite(std::shared_ptr<SpriteSheet> spriteSheet, SpriteSheet::SheetDesc sheetDesc);

        std::shared_ptr<SpriteSheet> m_spriteSheet;
        SpriteSheet::SheetDesc m_sheetDesc;
        std::optional<SpriteSheet::ClipDesc> m_currentClip;
        std::string m_currentClipName;
        int m_currentFrame = 0;
        float m_accumulatedMs = 0.0f;
        bool m_playing = false;
    };
}
