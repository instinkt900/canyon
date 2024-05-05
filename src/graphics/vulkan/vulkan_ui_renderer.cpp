#include "canyon.h"
#include "graphics/vulkan/vulkan_ui_renderer.h"
#include "graphics/vulkan/vulkan_graphics.h"
#include "graphics/vulkan/vulkan_font.h"
#include "utils/rect.h"

namespace graphics::vulkan {
    UIRenderer::UIRenderer(Graphics& graphics)
        : m_graphics(graphics) {
        m_drawColor.push({ 1.0f, 1.0f, 1.0f, 1.0f });
        m_blendMode.push(BlendMode::Replace);
    }

    void UIRenderer::PushBlendMode(moth_ui::BlendMode mode) {
        m_blendMode.push(FromMothUI(mode));
    }

    void UIRenderer::PopBlendMode() {
        if (!m_blendMode.empty()) {
            m_blendMode.pop();
        }
    }

    void UIRenderer::PushColor(moth_ui::Color const& color) {
        auto const modColor = m_drawColor.top() * ::FromMothUI(color);
        m_drawColor.push(modColor);
    }

    void UIRenderer::PopColor() {
        if (!m_drawColor.empty()) {
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

        auto const currentRect = m_clip.top();
        m_graphics.SetClipRect(&currentRect);
    }

    void UIRenderer::PopClip() {
        if (!m_clip.empty()) {
            m_clip.pop();
        }

        if (m_clip.empty()) {
            m_graphics.SetClipRect(nullptr);
        } else {
            auto const currentRect = m_clip.top();
            m_graphics.SetClipRect(&currentRect);
        }
    }

    void UIRenderer::RenderRect(moth_ui::IntRect const& rect) {
        m_graphics.SetBlendMode(m_blendMode.top());
        m_graphics.SetColor(m_drawColor.top());
        IntRect const internalRect = ::FromMothUI(rect);
        m_graphics.DrawRectF(static_cast<FloatRect>(internalRect));
    }

    void UIRenderer::RenderFilledRect(moth_ui::IntRect const& rect) {
        auto const internalRect = ::FromMothUI(rect);
        m_graphics.SetBlendMode(m_blendMode.top());
        m_graphics.SetColor(m_drawColor.top());
        m_graphics.DrawFillRectF(static_cast<FloatRect>(internalRect));
    }

    void UIRenderer::RenderImage(moth_ui::IImage& image, moth_ui::IntRect const& sourceRect, moth_ui::IntRect const& destRect, moth_ui::ImageScaleType scaleType, float scale) {
        auto& internalImage = static_cast<SubImage&>(image);
        auto const internalSourceRect = ::FromMothUI(sourceRect);
        auto const internalDestRect = ::FromMothUI(destRect);
        m_graphics.SetBlendMode(m_blendMode.top());
        m_graphics.SetColor(m_drawColor.top());
        m_graphics.DrawImage(internalImage, &internalSourceRect, &internalDestRect);
    }

    void UIRenderer::RenderText(std::string const& text, moth_ui::IFont& font, moth_ui::TextHorizAlignment horizontalAlignment, moth_ui::TextVertAlignment verticalAlignment, moth_ui::IntRect const& destRect) {
        auto& internalFont = static_cast<IFont&>(font);
        auto const internalDestRect = ::FromMothUI(destRect);
        m_graphics.SetBlendMode(BlendMode::Alpha);
        m_graphics.SetColor(m_drawColor.top());
        m_graphics.DrawText(text, internalFont, FromMothUI(horizontalAlignment), FromMothUI(verticalAlignment), internalDestRect);
    }
}
