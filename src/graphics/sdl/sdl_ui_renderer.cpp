#include "graphics/sdl/sdl_ui_renderer.h"
#include "graphics/sdl/sdl_image.h"
#include "graphics/sdl/sdl_font.h"
#include "graphics/sdl/sdl_utils.h"
#include "graphics/utils.h"

namespace graphics::sdl {
    UIRenderer::UIRenderer(SDL_Renderer& renderer)
        : m_renderer(renderer) {
        m_drawColor.push({ 1.0f, 1.0f, 1.0f, 1.0f });
        m_blendMode.push(BlendMode::Replace);
    }

    void UIRenderer::PushBlendMode(BlendMode mode) {
        m_blendMode.push(mode);
    }

    void UIRenderer::PopBlendMode() {
        if (m_blendMode.size() > 1) {
            m_blendMode.pop();
        }
    }

    void UIRenderer::PushColor(Color const& color) {
        auto const modColor = m_drawColor.top() * color;
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

    void UIRenderer::PushClip(IntRect const& rect) {
        if (m_clip.empty()) {
            m_clip.push(rect);
        } else {
            // want to clip rect within the current clip
            auto const parentRect = m_clip.top();
            auto const newRect = ClipRect(parentRect, rect);
            m_clip.push(newRect);
        }

        auto const currentRect = ToSDL(m_clip.top());
        SDL_RenderSetClipRect(&m_renderer, &currentRect);
    }

    void UIRenderer::PopClip() {
        m_clip.pop();

        if (m_clip.empty()) {
            SDL_RenderSetClipRect(&m_renderer, nullptr);
        } else {
            auto const currentRect = ToSDL(m_clip.top());
            SDL_RenderSetClipRect(&m_renderer, &currentRect);
        }
    }

    void UIRenderer::RenderRect(IntRect const& rect) {
        auto const sdlRect{ ToSDL(rect) };
        ColorComponents components{ m_drawColor.top() };
        SDL_SetRenderDrawBlendMode(&m_renderer, ToSDL(m_blendMode.top()));
        SDL_SetRenderDrawColor(&m_renderer, components.r, components.g, components.b, components.a);
        SDL_RenderDrawRect(&m_renderer, &sdlRect);
    }

    void UIRenderer::RenderFilledRect(IntRect const& rect) {
        auto const sdlRect{ ToSDL(rect) };
        ColorComponents components{ m_drawColor.top() };
        SDL_SetRenderDrawBlendMode(&m_renderer, ToSDL(m_blendMode.top()));
        SDL_SetRenderDrawColor(&m_renderer, components.r, components.g, components.b, components.a);
        SDL_RenderFillRect(&m_renderer, &sdlRect);
    }

    void UIRenderer::RenderImage(IImage& image, IntRect const& sourceRect, IntRect const& destRect, ImageScaleType scaleType, float scale) {
        auto const& internalImage = static_cast<Image&>(image);
        auto const texture = internalImage.GetTexture();
        auto const& textureSourceRect = internalImage.GetSourceRect();
        auto const sdlsourceRect{ ToSDL(MergeRects(textureSourceRect, sourceRect)) };
        ColorComponents const components{ m_drawColor.top() };
        SDL_SetTextureBlendMode(texture->GetImpl(), ToSDL(m_blendMode.top()));
        SDL_SetTextureColorMod(texture->GetImpl(), components.r, components.g, components.b);
        SDL_SetTextureAlphaMod(texture->GetImpl(), components.a);

        if (scaleType == ImageScaleType::Stretch) {
            auto const sdlDestRect{ ToSDL(destRect) };
            SDL_RenderCopy(&m_renderer, texture->GetImpl(), &sdlsourceRect, &sdlDestRect);
        } else if (scaleType == ImageScaleType::Tile) {
            // sdl doesnt have a tiling texture ability. we need to manually tile
            auto const imageWidth = static_cast<int>(image.GetWidth() * scale);
            auto const imageHeight = static_cast<int>(image.GetHeight() * scale);
            auto const sdlTotalDestRect{ ToSDL(destRect) };
            SDL_RenderSetClipRect(&m_renderer, &sdlTotalDestRect);
            for (auto y = destRect.topLeft.y; y < destRect.bottomRight.y; y += imageHeight) {
                for (auto x = destRect.topLeft.x; x < destRect.bottomRight.x; x += imageWidth) {
                    SDL_Rect sdlDestRect{ x, y, imageWidth, imageHeight };
                    SDL_RenderCopy(&m_renderer, texture->GetImpl(), &sdlsourceRect, &sdlDestRect);
                }
            }
            SDL_RenderSetClipRect(&m_renderer, nullptr);
        }
    }

    void UIRenderer::RenderText(std::string const& text, IFont& font, TextHorizAlignment horizontalAlignment, TextVertAlignment verticalAlignment, IntRect const& destRect) {
        auto const fcFont = static_cast<Font&>(font).GetFontObj();

        auto const destWidth = destRect.bottomRight.x - destRect.topLeft.x;
        auto const destHeight = destRect.bottomRight.y - destRect.topLeft.y;
        auto const textHeight = FC_GetColumnHeight(fcFont.get(), destWidth, "%s", text.c_str());

        auto x = static_cast<float>(destRect.topLeft.x);
        switch (horizontalAlignment) {
        case TextHorizAlignment::Left:
            break;
        case TextHorizAlignment::Center:
            x = x + destWidth / 2.0f;
            break;
        case TextHorizAlignment::Right:
            x = x + destWidth;
            break;
        }

        auto y = static_cast<float>(destRect.topLeft.y);
        switch (verticalAlignment) {
        case TextVertAlignment::Top:
            break;
        case TextVertAlignment::Middle:
            y = y + (destHeight - textHeight) / 2.0f;
            break;
        case TextVertAlignment::Bottom:
            y = y + destHeight - textHeight;
            break;
        }

        FC_Effect effect;
        effect.alignment = ToSDL(horizontalAlignment);
        effect.color = ToSDL(m_drawColor.top());
        effect.scale.x = 1.0f;
        effect.scale.y = 1.0f;

        FC_DrawColumnEffect(fcFont.get(), &m_renderer, x, y, destWidth, effect, "%s", text.c_str());
    }
}
