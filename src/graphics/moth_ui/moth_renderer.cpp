#include "common.h"
#include "graphics/moth_ui/moth_renderer.h"
#include "graphics/igraphics.h"
#include "graphics/moth_ui/moth_font.h"
#include "graphics/moth_ui/moth_image.h"
#include "graphics/moth_ui/utils.h"

namespace graphics {
    MothRenderer::MothRenderer(IGraphics& graphics)
        : m_graphics(graphics) {
        m_drawColor.push({ 1.0f, 1.0f, 1.0f, 1.0f });
        m_blendMode.push(BlendMode::Replace);
    }

    void MothRenderer::PushBlendMode(moth_ui::BlendMode mode) {
        m_blendMode.push(FromMothUI(mode));
    }

    void MothRenderer::PopBlendMode() {
        if (m_blendMode.size() > 1) {
            m_blendMode.pop();
        }
    }

    void MothRenderer::PushColor(moth_ui::Color const& color) {
        auto const modColor = m_drawColor.top() * FromMothUI(color);
        m_drawColor.push(modColor);
    }

    void MothRenderer::PopColor() {
        if (m_drawColor.size() > 1) {
            m_drawColor.pop();
        }
    }

    IntRect ClipRect(IntRect const& parentRect, IntRect const& childRect) {
        IntRect result;
        result.topLeft.x = std::max(parentRect.topLeft.x, childRect.topLeft.x);
        result.topLeft.y = std::max(parentRect.topLeft.y, childRect.topLeft.y);
        result.bottomRight.x = std::min(parentRect.bottomRight.x, childRect.bottomRight.x);
        result.bottomRight.y = std::min(parentRect.bottomRight.y, childRect.bottomRight.y);
        return result;
    }

    void MothRenderer::PushClip(moth_ui::IntRect const& rect) {
        if (m_clip.empty()) {
            m_clip.push(::FromMothUI(rect));
        } else {
            // want to clip rect within the current clip
            auto const parentRect = m_clip.top();
            auto const newRect = ClipRect(parentRect, ::FromMothUI(rect));
            m_clip.push(newRect);
        }

        m_graphics.SetClip(&m_clip.top());
    }

    void MothRenderer::PopClip() {
        m_clip.pop();

        if (m_clip.empty()) {
            m_graphics.SetClip(nullptr);
        } else {
            m_graphics.SetClip(&m_clip.top());
        }
    }

    void MothRenderer::RenderRect(moth_ui::IntRect const& rect) {
        auto const floatRect = static_cast<FloatRect>(::FromMothUI(rect));
        m_graphics.SetBlendMode(m_blendMode.top());
        m_graphics.SetColor(m_drawColor.top());
        m_graphics.DrawRectF(floatRect);
    }

    void MothRenderer::RenderFilledRect(moth_ui::IntRect const& rect) {
        auto const floatRect = static_cast<FloatRect>(::FromMothUI(rect));
        m_graphics.SetBlendMode(m_blendMode.top());
        m_graphics.SetColor(m_drawColor.top());
        m_graphics.DrawFillRectF(floatRect);
    }

    void MothRenderer::RenderImage(moth_ui::IImage& image, moth_ui::IntRect const& sourceRect, moth_ui::IntRect const& destRect, moth_ui::ImageScaleType scaleType, float scale) {
        m_graphics.SetBlendMode(m_blendMode.top());
        m_graphics.SetColor(m_drawColor.top());

        auto& mothImage = static_cast<graphics::MothImage&>(image);
        auto& internalImage = *mothImage.GetImage();
        auto const srcRect = ::FromMothUI(sourceRect);
        if (scaleType == moth_ui::ImageScaleType::Stretch) {
            m_graphics.DrawImage(internalImage, ::FromMothUI(destRect), &srcRect);
        } else if (scaleType == moth_ui::ImageScaleType::Tile) {
            m_graphics.DrawImageTiled(internalImage, ::FromMothUI(destRect), &srcRect, scale);
        }
    }

    void MothRenderer::RenderText(std::string const& text, moth_ui::IFont& font, moth_ui::TextHorizAlignment horizontalAlignment, moth_ui::TextVertAlignment verticalAlignment, moth_ui::IntRect const& destRect) {
        m_graphics.SetBlendMode(m_blendMode.top());
        m_graphics.SetColor(m_drawColor.top());

        auto& fcFont = static_cast<MothFont&>(font);
        auto& internalFont = *fcFont.GetInternalFont();
        m_graphics.DrawText(text, internalFont, FromMothUI(horizontalAlignment), FromMothUI(verticalAlignment), ::FromMothUI(destRect));
    }
}
