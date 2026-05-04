#include "moth_graphics/graphics/moth_ui/moth_renderer.h"
#include "moth_graphics/graphics/igraphics.h"
#include "moth_graphics/graphics/moth_ui/moth_font.h"
#include "moth_graphics/graphics/moth_ui/moth_image.h"

namespace moth_graphics::graphics {
    namespace {
        TextureFilter ToGraphicsFilter(moth_ui::TextureFilter f) {
            return f == moth_ui::TextureFilter::Nearest
                ? TextureFilter::Nearest
                : TextureFilter::Linear;
        }
    }

    MothRenderer::MothRenderer(IGraphics& graphics)
        : m_graphics(graphics) {
        m_drawColor.push({ 1.0f, 1.0f, 1.0f, 1.0f });
        m_blendMode.push(BlendMode::Replace);
        m_transform.push(moth_ui::FloatMat4x4::Identity());
        m_graphics.SetTransform(m_transform.top());
        m_textureFilter.push(moth_ui::TextureFilter::Linear);
    }

    void MothRenderer::PushBlendMode(moth_ui::BlendMode mode) {
        m_blendMode.push(mode);
    }

    void MothRenderer::PopBlendMode() {
        if (m_blendMode.size() > 1) {
            m_blendMode.pop();
        }
    }

    void MothRenderer::PushColor(moth_ui::Color const& color) {
        auto const modColor = m_drawColor.top() * color;
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
        result.bottomRight.x = std::max(result.topLeft.x, result.bottomRight.x);
        result.bottomRight.y = std::max(result.topLeft.y, result.bottomRight.y);
        return result;
    }

    void MothRenderer::PushTransform(moth_ui::FloatMat4x4 const& transform) {
        auto const combined = m_transform.top() * transform;
        m_transform.push(combined);
        m_graphics.SetTransform(combined);
    }

    void MothRenderer::PopTransform() {
        if (m_transform.size() > 1) {
            m_transform.pop();
            m_graphics.SetTransform(m_transform.top());
        }
    }

    void MothRenderer::PushClip(moth_ui::IntRect const& rect) {
        if (m_clip.empty()) {
            m_clip.push(rect);
        } else {
            // want to clip rect within the current clip
            auto const parentRect = m_clip.top();
            auto const newRect = ClipRect(parentRect, rect);
            m_clip.push(newRect);
        }

        m_graphics.SetClip(&m_clip.top());
    }

    void MothRenderer::PopClip() {
        if (m_clip.empty()) {
            return;
        }
        m_clip.pop();

        if (m_clip.empty()) {
            m_graphics.SetClip(nullptr);
        } else {
            m_graphics.SetClip(&m_clip.top());
        }
    }

    void MothRenderer::PushTextureFilter(moth_ui::TextureFilter filter) {
        m_textureFilter.push(filter);
    }

    void MothRenderer::PopTextureFilter() {
        if (m_textureFilter.size() > 1) {
            m_textureFilter.pop();
        }
    }

    void MothRenderer::RenderRect(moth_ui::IntRect const& rect) {
        m_graphics.SetBlendMode(m_blendMode.top());
        m_graphics.SetColor(m_drawColor.top());
        m_graphics.DrawRectF(static_cast<FloatRect>(rect));
    }

    void MothRenderer::RenderFilledRect(moth_ui::IntRect const& rect) {
        m_graphics.SetBlendMode(m_blendMode.top());
        m_graphics.SetColor(m_drawColor.top());
        m_graphics.DrawFillRectF(static_cast<FloatRect>(rect));
    }

    void MothRenderer::RenderImage(moth_ui::IImage const& image, moth_ui::IntRect const& sourceRect, moth_ui::IntRect const& destRect, moth_ui::ImageScaleType scaleType, float scale) {
        m_graphics.SetBlendMode(m_blendMode.top());
        m_graphics.SetColor(m_drawColor.top());

        auto const* mothImagePtr = dynamic_cast<MothImage const*>(&image);
        if (mothImagePtr == nullptr) {
            return;
        }
        auto const& internalImage = mothImagePtr->GetImage();
        if (!internalImage) {
            return;
        }
        auto const srcRect = sourceRect;

        if (destRect.w() <= 0 || destRect.h() <= 0) {
            return;
        }

        if (auto texture = internalImage.GetTexture()) {
            auto const gfxFilter = ToGraphicsFilter(m_textureFilter.top());
            texture->SetFilter(gfxFilter, gfxFilter);
        }

        if (scaleType == moth_ui::ImageScaleType::Stretch) {
            m_graphics.DrawImage(internalImage, destRect, &srcRect);
        } else if (scaleType == moth_ui::ImageScaleType::Tile) {
            m_graphics.DrawImageTiled(internalImage, destRect, &srcRect, scale);
        }
    }

    void MothRenderer::RenderText(std::string_view text, moth_ui::IFont& font, moth_ui::TextHorizAlignment horizontalAlignment, moth_ui::TextVertAlignment verticalAlignment, moth_ui::IntRect const& destRect) {
        m_graphics.SetBlendMode(m_blendMode.top());
        m_graphics.SetColor(m_drawColor.top());

        auto* fcFontPtr = dynamic_cast<MothFont*>(&font);
        if (fcFontPtr == nullptr) {
            return;
        }
        auto internalFontPtr = fcFontPtr->GetInternalFont();
        if (internalFontPtr == nullptr) {
            return;
        }
        auto& internalFont = *internalFontPtr;
        m_graphics.DrawText(text, internalFont, destRect, horizontalAlignment, verticalAlignment);
    }

    void MothRenderer::SetRendererLogicalSize(moth_ui::IntVec2 const& size) {
        m_graphics.SetLogicalSize(size);
    }
}
