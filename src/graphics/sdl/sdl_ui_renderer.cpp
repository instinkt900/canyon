#include "canyon.h"
#include "graphics/sdl/sdl_ui_renderer.h"
#include "graphics/sdl/sdl_image.h"
#include "graphics/sdl/sdl_font.h"
#include "graphics/sdl/sdl_utils.h"
#include "graphics/utils.h"
#include "utils/conversions.h"

namespace graphics::sdl {
    UIRenderer::UIRenderer(IGraphics& graphics)
        : m_graphics(graphics) {
        m_drawColor.push({ 1.0f, 1.0f, 1.0f, 1.0f });
        m_blendMode.push(BlendMode::Replace);
    }

    void UIRenderer::PushBlendMode(moth_ui::BlendMode mode) {
        m_blendMode.push(FromMothUI(mode));
    }

    void UIRenderer::PopBlendMode() {
        if (m_blendMode.size() > 1) {
            m_blendMode.pop();
        }
    }

    void UIRenderer::PushColor(moth_ui::Color const& color) {
        auto const modColor = m_drawColor.top() * ::FromMothUI(color);
        m_drawColor.push(modColor);
    }

    void UIRenderer::PopColor() {
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

    void UIRenderer::PushClip(moth_ui::IntRect const& rect) {
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

    void UIRenderer::PopClip() {
        m_clip.pop();

        if (m_clip.empty()) {
            m_graphics.SetClip(nullptr);
        } else {
            m_graphics.SetClip(&m_clip.top());
        }
    }

    void UIRenderer::RenderRect(moth_ui::IntRect const& rect) {
        auto const floatRect = static_cast<FloatRect>(::FromMothUI(rect));
        m_graphics.SetBlendMode(m_blendMode.top());
        m_graphics.SetColor(m_drawColor.top());
        m_graphics.DrawRectF(floatRect);
    }

    void UIRenderer::RenderFilledRect(moth_ui::IntRect const& rect) {
        auto const floatRect = static_cast<FloatRect>(::FromMothUI(rect));
        m_graphics.SetBlendMode(m_blendMode.top());
        m_graphics.SetColor(m_drawColor.top());
        m_graphics.DrawFillRectF(floatRect);
    }

    void UIRenderer::RenderImage(moth_ui::IImage& image, moth_ui::IntRect const& sourceRect, moth_ui::IntRect const& destRect, moth_ui::ImageScaleType scaleType, float scale) {
        m_graphics.SetBlendMode(m_blendMode.top());
        m_graphics.SetColor(m_drawColor.top());

        auto& internalImage = static_cast<graphics::IImage&>(image);
        auto const srcRect = ::FromMothUI(sourceRect);
        if (scaleType == moth_ui::ImageScaleType::Stretch) {
            m_graphics.DrawImage(internalImage, ::FromMothUI(destRect), &srcRect);
        } else if (scaleType == moth_ui::ImageScaleType::Tile) {
            m_graphics.DrawImageTiled(internalImage, ::FromMothUI(destRect), &srcRect, scale);
        }
    }

    void UIRenderer::RenderText(std::string const& text, moth_ui::IFont& font, moth_ui::TextHorizAlignment horizontalAlignment, moth_ui::TextVertAlignment verticalAlignment, moth_ui::IntRect const& destRect) {
        auto& fcFont = static_cast<Font&>(font);
        m_graphics.DrawText(text, fcFont, FromMothUI(horizontalAlignment), FromMothUI(verticalAlignment), ::FromMothUI(destRect));
    }
}
