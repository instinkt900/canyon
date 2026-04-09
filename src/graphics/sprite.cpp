#include "common.h"
#include "moth_graphics/graphics/sprite.h"

namespace moth_graphics::graphics {
    std::unique_ptr<Sprite> Sprite::Create(std::shared_ptr<SpriteSheet> spriteSheet) {
        if (!spriteSheet) {
            spdlog::error("Sprite::Create: spriteSheet must not be null");
            return nullptr;
        }
        if (spriteSheet->GetFrameCount() <= 0) {
            spdlog::error("Sprite::Create: spriteSheet has no frames");
            return nullptr;
        }
        return std::unique_ptr<Sprite>(new Sprite(std::move(spriteSheet)));
    }

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

        SpriteSheet::ClipDesc clipDesc;
        if (m_spriteSheet->GetClipDesc(name, clipDesc)) {
            m_currentClip = std::move(clipDesc);
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
        SpriteSheet::FrameEntry entry;
        if (m_spriteSheet->GetFrameDesc(GetCurrentFrame(), entry)) {
            return entry.rect;
        }
        return {};
    }

    int Sprite::GetWidth() const {
        SpriteSheet::FrameEntry entry;
        if (m_spriteSheet->GetFrameDesc(GetCurrentFrame(), entry)) {
            return entry.rect.bottomRight.x - entry.rect.topLeft.x;
        }
        return 0;
    }

    int Sprite::GetHeight() const {
        SpriteSheet::FrameEntry entry;
        if (m_spriteSheet->GetFrameDesc(GetCurrentFrame(), entry)) {
            return entry.rect.bottomRight.y - entry.rect.topLeft.y;
        }
        return 0;
    }

    IImage* Sprite::GetImage() const {
        return m_spriteSheet->GetImage().get();
    }
}
