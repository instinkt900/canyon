#include "canyon.h"
#include "graphics/vulkan/vulkan_ui_renderer.h"
#include "graphics/vulkan/vulkan_graphics.h"
#include "graphics/vulkan/vulkan_font.h"

namespace graphics::vulkan {
    UIRenderer::UIRenderer(Graphics& graphics)
        : m_graphics(graphics) {
        m_drawColor.push({ 1.0f, 1.0f, 1.0f, 1.0f });
        m_blendMode.push(BlendMode::Replace);
    }

    void UIRenderer::PushBlendMode(BlendMode mode) {
        m_blendMode.push(mode);
    }

    void UIRenderer::PopBlendMode() {
        if (!m_blendMode.empty()) {
            m_blendMode.pop();
        }
    }

    void UIRenderer::PushColor(Color const& color) {
        auto const modColor = m_drawColor.top() * color;
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

    void UIRenderer::PushClip(IntRect const& rect) {
        if (m_clip.empty()) {
            m_clip.push(rect);
        } else {
            // want to clip rect within the current clip
            auto const parentRect = m_clip.top();
            auto const newRect = ClipRect(parentRect, rect);
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

    void UIRenderer::RenderRect(IntRect const& rect) {
        m_graphics.SetBlendMode(m_blendMode.top());
        m_graphics.SetColor(m_drawColor.top());
        m_graphics.DrawRectF(static_cast<FloatRect>(rect));
    }

    void UIRenderer::RenderFilledRect(IntRect const& rect) {
        m_graphics.SetBlendMode(m_blendMode.top());
        m_graphics.SetColor(m_drawColor.top());
        m_graphics.DrawFillRectF(static_cast<FloatRect>(rect));
    }

    void UIRenderer::RenderImage(IImage& image, IntRect const& sourceRect, IntRect const& destRect, ImageScaleType scaleType, float scale) {
        m_graphics.SetBlendMode(m_blendMode.top());
        m_graphics.SetColor(m_drawColor.top());
        m_graphics.DrawImage(image, &sourceRect, &destRect);
    }

    void UIRenderer::RenderText(std::string const& text, IFont& font, TextHorizAlignment horizontalAlignment, TextVertAlignment verticalAlignment, IntRect const& destRect) {
        m_graphics.SetBlendMode(BlendMode::Alpha);
        m_graphics.SetColor(m_drawColor.top());
        m_graphics.DrawText(text, font, horizontalAlignment, verticalAlignment, destRect);
    }
}
