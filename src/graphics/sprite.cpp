#include "common.h"
#include "moth_graphics/graphics/sprite.h"
#include "moth_graphics/graphics/igraphics.h"

namespace moth_graphics::graphics {
    Sprite::Sprite(std::shared_ptr<SpriteSheet> spriteSheet)
        : m_spriteSheet(std::move(spriteSheet)) {
    }

    void Sprite::SetClip(std::string_view name) {
        m_accumulatedMs = 0.0f;
        m_currentFrame = 0;
        m_currentClip.reset();
        m_currentClipName.clear();

        if (name.empty()) {
            m_playing = false;
            return;
        }

        auto clipDesc = m_spriteSheet->GetClipDesc(name);
        if (clipDesc) {
            m_currentClip = std::move(*clipDesc);
            m_currentClipName = name;
        } else {
            spdlog::warn("Sprite::SetClip: clip '{}' not found", name);
            m_playing = false;
        }
    }

    void Sprite::SetFrame(int frame) {
        m_playing = false;
        m_currentClip.reset();
        m_currentClipName.clear();
        m_accumulatedMs = 0.0f;
        m_currentFrame = std::clamp(frame, 0, m_spriteSheet->GetFrameCount() - 1);
    }

    void Sprite::SetPlaying(bool playing) {
        if (m_currentClip.has_value()) {
            m_playing = playing;
        }
    }

    void Sprite::Update(uint32_t ticks) {
        if (!m_playing || !m_currentClip.has_value() || m_currentClip->frames.empty()) {
            return;
        }

        m_accumulatedMs += static_cast<float>(ticks);

        while (m_playing) {
            int const durationMs = m_currentClip->frames[static_cast<size_t>(m_currentFrame)].durationMs;
            if (durationMs <= 0 || m_accumulatedMs < static_cast<float>(durationMs)) {
                break;
            }
            m_accumulatedMs -= static_cast<float>(durationMs);
            ++m_currentFrame;
            int const lastStep = static_cast<int>(m_currentClip->frames.size()) - 1;
            if (m_currentFrame > lastStep) {
                switch (m_currentClip->loop) {
                case SpriteSheet::LoopType::Loop:
                    m_currentFrame = 0;
                    break;
                case SpriteSheet::LoopType::Reset:
                    m_accumulatedMs = 0.0f;
                    m_currentFrame = 0;
                    m_playing = false;
                    break;
                case SpriteSheet::LoopType::Stop:
                    m_accumulatedMs = 0.0f;
                    m_currentFrame = lastStep;
                    m_playing = false;
                    break;
                }
            }
        }
    }

    int Sprite::GetCurrentFrame() const {
        if (m_currentClip.has_value() && !m_currentClip->frames.empty()) {
            return m_currentClip->frames[static_cast<size_t>(m_currentFrame)].frameIndex;
        }
        return m_currentFrame;
    }

    IntRect Sprite::GetCurrentFrameRect() const {
        auto entry = m_spriteSheet->GetFrameDesc(GetCurrentFrame());
        if (entry) {
            return entry->rect;
        }
        return {};
    }

    IntVec2 Sprite::GetCurrentFramePivot() const {
        auto entry = m_spriteSheet->GetFrameDesc(GetCurrentFrame());
        if (entry) {
            return entry->pivot;
        }
        return {};
    }

    int Sprite::GetWidth() const {
        auto entry = m_spriteSheet->GetFrameDesc(GetCurrentFrame());
        if (entry) {
            return entry->rect.bottomRight.x - entry->rect.topLeft.x;
        }
        return 0;
    }

    int Sprite::GetHeight() const {
        auto entry = m_spriteSheet->GetFrameDesc(GetCurrentFrame());
        if (entry) {
            return entry->rect.bottomRight.y - entry->rect.topLeft.y;
        }
        return 0;
    }

    Image const& Sprite::GetImage() const {
        return m_spriteSheet->GetImage();
    }

    void DrawSprite(IGraphics& graphics, Sprite const& sprite, IntRect const& destRect) {
        auto const& image = sprite.GetImage();
        if (image) {
            auto const frameRect = sprite.GetCurrentFrameRect();
            graphics.DrawImage(image, destRect, &frameRect);
        }
    }

    void DrawSprite(IGraphics& graphics, Sprite const& sprite, IntVec2 const& pos, FloatVec2 const& pivot) {
        auto const& image = sprite.GetImage();
        if (image) {
            auto const frameRect = sprite.GetCurrentFrameRect();
            IntRect const destRect = MakeRect(
                pos.x - static_cast<int>(pivot.x * static_cast<float>(frameRect.w())),
                pos.y - static_cast<int>(pivot.y * static_cast<float>(frameRect.h())),
                frameRect.w(),
                frameRect.h());
            graphics.DrawImage(image, destRect, &frameRect);
        }
    }

    void DrawSprite(IGraphics& graphics, Sprite const& sprite, IntVec2 const& pos) {
        auto const& image = sprite.GetImage();
        if (image) {
            auto const frameRect = sprite.GetCurrentFrameRect();
            auto const framePivot = sprite.GetCurrentFramePivot();
            IntRect const destRect = MakeRect(
                pos.x - framePivot.x,
                pos.y - framePivot.y,
                frameRect.w(),
                frameRect.h());
            graphics.DrawImage(image, destRect, &frameRect);
        }
    }
}
