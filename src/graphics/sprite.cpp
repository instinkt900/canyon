#include "common.h"
#include "moth_graphics/graphics/sprite.h"

namespace moth_graphics::graphics {
    std::unique_ptr<Sprite> Sprite::Create(std::shared_ptr<SpriteSheet> spriteSheet) {
        if (!spriteSheet) {
            spdlog::error("Sprite::Create: spriteSheet must not be null");
            return nullptr;
        }

        SpriteSheet::SheetDesc desc;
        spriteSheet->GetSheetDesc(desc);
        if (desc.SheetCells.x <= 0 || desc.SheetCells.y <= 0 ||
            desc.FrameDimensions.x <= 0 || desc.FrameDimensions.y <= 0) {
            spdlog::error("Sprite::Create: invalid sheet descriptor (cells={}x{} frame={}x{})",
                          desc.SheetCells.x, desc.SheetCells.y,
                          desc.FrameDimensions.x, desc.FrameDimensions.y);
            return nullptr;
        }

        return std::unique_ptr<Sprite>(new Sprite(std::move(spriteSheet), desc));
    }

    Sprite::Sprite(std::shared_ptr<SpriteSheet> spriteSheet, SpriteSheet::SheetDesc sheetDesc)
        : m_spriteSheet(std::move(spriteSheet))
        , m_sheetDesc(sheetDesc) {
    }

    void Sprite::SetClip(std::string_view name) {
        m_accumulatedMs = 0.0f;
        m_currentFrame = 0;
        m_currentClip.reset();
        m_currentClipName.clear();

        SpriteSheet::ClipDesc clipDesc;
        if (m_spriteSheet->GetClipDesc(name, clipDesc)) {
            m_currentClip = clipDesc;
            m_currentClipName = name;
            m_currentFrame = m_currentClip->Start;
        } else if (!name.empty()) {
            spdlog::warn("Sprite::SetClip: clip '{}' not found", name);
            m_playing = false;
        }
    }

    void Sprite::SetFrame(int frame) {
        m_playing = false;
        m_currentClip.reset();
        m_currentClipName.clear();
        m_accumulatedMs = 0.0f;
        int const maxFrames = m_sheetDesc.MaxFrames > 0 ? m_sheetDesc.MaxFrames
                                                        : m_sheetDesc.SheetCells.x * m_sheetDesc.SheetCells.y;
        m_currentFrame = std::clamp(frame, 0, maxFrames - 1);
    }

    void Sprite::SetPlaying(bool playing) {
        if (m_currentClip.has_value()) {
            m_playing = playing;
        }
    }

    void Sprite::Update(uint32_t ticks) {
        if (!m_playing || !m_currentClip.has_value()) {
            return;
        }
        if (m_currentClip->FPS <= 0) {
            return;
        }

        float const frameDurationMs = 1000.0f / static_cast<float>(m_currentClip->FPS);
        m_accumulatedMs += static_cast<float>(ticks);

        int const framesToAdvance = static_cast<int>(m_accumulatedMs / frameDurationMs);
        if (framesToAdvance == 0) {
            return;
        }
        m_accumulatedMs -= static_cast<float>(framesToAdvance) * frameDurationMs;

        m_currentFrame += framesToAdvance;
        if (m_currentFrame > m_currentClip->End) {
            int const clipLength = m_currentClip->End - m_currentClip->Start + 1;
            switch (m_currentClip->Loop) {
            case SpriteSheet::LoopType::Loop:
                m_currentFrame = m_currentClip->Start + (m_currentFrame - m_currentClip->Start) % clipLength;
                break;
            case SpriteSheet::LoopType::Reset:
                m_accumulatedMs = 0.0f;
                m_currentFrame = m_currentClip->Start;
                m_playing = false;
                break;
            case SpriteSheet::LoopType::Stop:
                m_accumulatedMs = 0.0f;
                m_currentFrame = m_currentClip->End;
                m_playing = false;
                break;
            default:
                m_playing = false;
                break;
            }
        }
    }

    IntRect Sprite::GetCurrentFrameRect() const {
        int const col = m_currentFrame % m_sheetDesc.SheetCells.x;
        int const row = m_currentFrame / m_sheetDesc.SheetCells.x;
        return MakeRect(col * m_sheetDesc.FrameDimensions.x,
                        row * m_sheetDesc.FrameDimensions.y,
                        m_sheetDesc.FrameDimensions.x,
                        m_sheetDesc.FrameDimensions.y);
    }

    IImage* Sprite::GetImage() const {
        return m_spriteSheet->GetImage().get();
    }
}
