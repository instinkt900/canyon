#include "common.h"
#include "sdl_graphics.h"
#include "sdl_font.h"
#include "sdl_target.h"
#include "sdl_texture.h"
#include "sdl_utils.hpp"
#include "moth_graphics/platform/sdl/sdl_window.h"
#include "../utils.h"

namespace moth_graphics::graphics::sdl {
    Graphics::Graphics(SurfaceContext& context)
        : m_surfaceContext(context)
        , m_drawColor(graphics::BasicColors::White) {
    }

    Graphics::~Graphics() = default;

    void Graphics::Begin() {
    }

    void Graphics::End() {
    }

    void Graphics::SetBlendMode(graphics::BlendMode mode) {
        SDL_SetRenderDrawBlendMode(m_surfaceContext.GetRenderer(), ToSDL(mode));
        m_blendMode = mode;
    }

    void Graphics::SetColor(graphics::Color const& color) {
        ColorComponents components(color);
        SDL_SetRenderDrawColor(m_surfaceContext.GetRenderer(), components.r, components.g, components.b, components.a);
        m_drawColor = color;
    }

    void Graphics::Clear() {
        SDL_RenderClear(m_surfaceContext.GetRenderer());
    }

    FloatMat4x4 Graphics::CurrentTransform() const {
        return m_currentTransform;
    }

    void Graphics::SetTransform(FloatMat4x4 const& transform) {
        m_currentTransform = transform;
    }

    void Graphics::DrawImage(Image const& image, IntVec2 const& pos, FloatVec2 const& pivot) {
        auto const imageWidth = image.GetWidth();
        auto const imageHeight = image.GetHeight();
        auto const offsetX = static_cast<int>(static_cast<float>(imageWidth) * pivot.x);
        auto const offsetY = static_cast<int>(static_cast<float>(imageHeight) * pivot.y);
        IntRect destRect = MakeRect(pos.x, pos.y, imageWidth, imageHeight);
        DrawImage(image, destRect - IntVec2{ offsetX, offsetY }, nullptr);
    }

    void Graphics::DrawImage(Image const& image, IntRect const& destRect, IntRect const* sourceRect) {
        auto sdlTexture = std::dynamic_pointer_cast<Texture>(image.GetTexture());
        if (!sdlTexture) {
            return;
        }
        auto const& textureSourceRect = image.GetSourceRect();

        ColorComponents const components{ m_drawColor };
        SDL_SetTextureBlendMode(sdlTexture->GetSDLTexture()->GetImpl(), ToSDL(m_blendMode));
        SDL_SetTextureColorMod(sdlTexture->GetSDLTexture()->GetImpl(), components.r, components.g, components.b);
        SDL_SetTextureAlphaMod(sdlTexture->GetSDLTexture()->GetImpl(), components.a);

        SDL_Rect sdlSrcRect = ToSDL(sourceRect != nullptr ? *sourceRect : textureSourceRect);

        // Negative src dimensions mean the caller wants a mirrored draw.
        // SDL doesn't support negative-dimension rects; normalize and flip instead.
        bool flipH = false;
        bool flipV = false;
        if (sdlSrcRect.w < 0) {
            sdlSrcRect.x += sdlSrcRect.w;
            sdlSrcRect.w = -sdlSrcRect.w;
            flipH = true;
        }
        if (sdlSrcRect.h < 0) {
            sdlSrcRect.y += sdlSrcRect.h;
            sdlSrcRect.h = -sdlSrcRect.h;
            flipV = true;
        }
        SDL_RendererFlip flip = SDL_FLIP_NONE;
        if (flipH && flipV) {
            flip = static_cast<SDL_RendererFlip>(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
        } else if (flipH) {
            flip = SDL_FLIP_HORIZONTAL;
        } else if (flipV) {
            flip = SDL_FLIP_VERTICAL;
        }

        auto const t = CurrentTransform();
        float const localCenterX = static_cast<float>(destRect.topLeft.x + destRect.bottomRight.x) * 0.5f;
        float const localCenterY = static_cast<float>(destRect.topLeft.y + destRect.bottomRight.y) * 0.5f;
        auto const worldCenter = t.TransformPoint({ localCenterX, localCenterY });
        float const w = static_cast<float>(destRect.bottomRight.x - destRect.topLeft.x);
        float const h = static_cast<float>(destRect.bottomRight.y - destRect.topLeft.y);
        SDL_Rect const sdlDstRect{
            static_cast<int>(worldCenter.x - (w * 0.5f)),
            static_cast<int>(worldCenter.y - (h * 0.5f)),
            static_cast<int>(w),
            static_cast<int>(h),
        };
        double const rotation = static_cast<double>(t.GetRotationDegrees());

        SDL_RenderCopyEx(m_surfaceContext.GetRenderer(),
                         sdlTexture->GetSDLTexture()->GetImpl(),
                         &sdlSrcRect,
                         &sdlDstRect,
                         rotation,
                         nullptr,
                         flip);
    }

    void Graphics::DrawImageTiled(Image const& image, IntRect const& destRect, IntRect const* sourceRect, float scale) {
        auto const imageWidth = static_cast<int>(static_cast<float>(image.GetWidth()) * scale);
        auto const imageHeight = static_cast<int>(static_cast<float>(image.GetHeight()) * scale);
        if (imageWidth <= 0 || imageHeight <= 0) {
            return;
        }
        for (auto y = destRect.topLeft.y; y < destRect.bottomRight.y; y += imageHeight) {
            for (auto x = destRect.topLeft.x; x < destRect.bottomRight.x; x += imageWidth) {
                DrawImage(image, IntRect{ { x, y }, { x + imageWidth, y + imageHeight } }, sourceRect);
            }
        }
    }

    void Graphics::DrawRectF(FloatRect const& rect) {
        auto const t = CurrentTransform();
        auto const tl = t.TransformPoint({ rect.topLeft.x, rect.topLeft.y });
        auto const tr = t.TransformPoint({ rect.bottomRight.x, rect.topLeft.y });
        auto const br = t.TransformPoint({ rect.bottomRight.x, rect.bottomRight.y });
        auto const bl = t.TransformPoint({ rect.topLeft.x, rect.bottomRight.y });
        SDL_FPoint const pts[5] = {
            { tl.x, tl.y },
            { tr.x, tr.y },
            { br.x, br.y },
            { bl.x, bl.y },
            { tl.x, tl.y },
        };
        SDL_RenderDrawLinesF(m_surfaceContext.GetRenderer(), pts, 5);
    }

    void Graphics::DrawFillRectF(FloatRect const& rect) {
        auto const t = CurrentTransform();
        auto const tl = t.TransformPoint({ rect.topLeft.x, rect.topLeft.y });
        auto const tr = t.TransformPoint({ rect.bottomRight.x, rect.topLeft.y });
        auto const br = t.TransformPoint({ rect.bottomRight.x, rect.bottomRight.y });
        auto const bl = t.TransformPoint({ rect.topLeft.x, rect.bottomRight.y });
        ColorComponents const components{ m_drawColor };
        SDL_Color const sdlColor{ components.r, components.g, components.b, components.a };
        SDL_Vertex const sdlVertices[6] = {
            { { tl.x, tl.y }, sdlColor, { 0.0f, 0.0f } },
            { { tr.x, tr.y }, sdlColor, { 0.0f, 0.0f } },
            { { bl.x, bl.y }, sdlColor, { 0.0f, 0.0f } },
            { { tr.x, tr.y }, sdlColor, { 0.0f, 0.0f } },
            { { br.x, br.y }, sdlColor, { 0.0f, 0.0f } },
            { { bl.x, bl.y }, sdlColor, { 0.0f, 0.0f } },
        };
        SDL_RenderGeometry(m_surfaceContext.GetRenderer(), nullptr, sdlVertices, 6, nullptr, 0);
    }

    void Graphics::DrawLineF(FloatVec2 const& p0, FloatVec2 const& p1) {
        auto const t = CurrentTransform();
        auto const wp0 = t.TransformPoint(p0);
        auto const wp1 = t.TransformPoint(p1);
        SDL_RenderDrawLineF(m_surfaceContext.GetRenderer(), wp0.x, wp0.y, wp1.x, wp1.y);
    }

    void Graphics::DrawText(std::string_view text, graphics::IFont& font, IntRect const& destRect, graphics::TextHorizAlignment horizontalAlignment, graphics::TextVertAlignment verticalAlignment) {
        std::string const textStr(text);
        auto* sdlFont = dynamic_cast<Font*>(&font);
        if (sdlFont == nullptr) {
            spdlog::warn("DrawText: font is not an SDL Font; skipping");
            return;
        }
        auto const fcFont = sdlFont->GetFontObj();

        auto const destWidth = destRect.bottomRight.x - destRect.topLeft.x;
        auto const destHeight = destRect.bottomRight.y - destRect.topLeft.y;
        auto const textHeight = FC_GetColumnHeight(fcFont.get(), destWidth, "%s", textStr.c_str()); // NOLINT(cppcoreguidelines-pro-type-vararg)

        // local x/y: position within destRect for the text origin, accounting for alignment
        auto x = 0.0f;
        switch (horizontalAlignment) {
        case graphics::TextHorizAlignment::Left:
            break;
        case graphics::TextHorizAlignment::Center:
            x = static_cast<float>(destWidth) / 2.0f;
            break;
        case graphics::TextHorizAlignment::Right:
            x = static_cast<float>(destWidth);
            break;
        }

        auto y = 0.0f;
        switch (verticalAlignment) {
        case graphics::TextVertAlignment::Top:
            break;
        case graphics::TextVertAlignment::Middle:
            y = (static_cast<float>(destHeight) - static_cast<float>(textHeight)) / 2.0f;
            break;
        case graphics::TextVertAlignment::Bottom:
            y = static_cast<float>(destHeight) - static_cast<float>(textHeight);
            break;
        }

        FC_Effect effect;
        effect.alignment = ToSDL(horizontalAlignment);
        effect.color = ToSDL(m_drawColor);
        effect.scale.x = 1.0f;
        effect.scale.y = 1.0f;

        auto const t = CurrentTransform();
        double const rotation = static_cast<double>(t.GetRotationDegrees());

        if (rotation == 0.0) {
            // Fast path: no rotation — draw directly at the transformed world position.
            auto const worldPos = t.TransformPoint({ static_cast<float>(destRect.topLeft.x) + x,
                                                     static_cast<float>(destRect.topLeft.y) + y });
            FC_DrawColumnEffect(fcFont.get(), m_surfaceContext.GetRenderer(), worldPos.x, worldPos.y, destWidth, effect, "%s", textStr.c_str()); // NOLINT(cppcoreguidelines-pro-type-vararg)
        } else {
            // Rotation path: render text to a scratch texture, then draw it rotated.
            // Grow the scratch texture only when the current one is too small.
            if (!m_textScratchTexture || destWidth > m_textScratchWidth || destHeight > m_textScratchHeight) {
                m_textScratchWidth = std::max(destWidth, m_textScratchWidth);
                m_textScratchHeight = std::max(destHeight, m_textScratchHeight);
                m_textScratchTexture = CreateTextureRef(SDL_CreateTexture(m_surfaceContext.GetRenderer(),
                                                                          SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, m_textScratchWidth, m_textScratchHeight));
                SDL_SetTextureBlendMode(m_textScratchTexture->GetImpl(), SDL_BLENDMODE_BLEND);
            }

            SDL_Texture* prevTarget = SDL_GetRenderTarget(m_surfaceContext.GetRenderer());
            SDL_SetRenderTarget(m_surfaceContext.GetRenderer(), m_textScratchTexture->GetImpl());
            SDL_SetRenderDrawColor(m_surfaceContext.GetRenderer(), 0, 0, 0, 0);
            SDL_RenderClear(m_surfaceContext.GetRenderer());

            FC_DrawColumnEffect(fcFont.get(), m_surfaceContext.GetRenderer(), x, y, destWidth, effect, "%s", textStr.c_str()); // NOLINT(cppcoreguidelines-pro-type-vararg)

            SDL_SetRenderTarget(m_surfaceContext.GetRenderer(), prevTarget);

            // Draw the scratch texture at the world-space center of destRect, rotated.
            float const localCenterX = static_cast<float>(destWidth) * 0.5f;
            float const localCenterY = static_cast<float>(destHeight) * 0.5f;
            auto const worldCenter = t.TransformPoint({ static_cast<float>(destRect.topLeft.x) + localCenterX,
                                                        static_cast<float>(destRect.topLeft.y) + localCenterY });
            SDL_Rect const srcRect{ 0, 0, destWidth, destHeight };
            SDL_Rect const dstRect{
                static_cast<int>(worldCenter.x - localCenterX),
                static_cast<int>(worldCenter.y - localCenterY),
                destWidth,
                destHeight,
            };
            SDL_RenderCopyEx(m_surfaceContext.GetRenderer(), m_textScratchTexture->GetImpl(),
                             &srcRect, &dstRect, rotation, nullptr, SDL_FLIP_NONE);
        }
    }

    void Graphics::SetClip(IntRect const* rect) {
        if (rect != nullptr) {
            auto const currentRect = ToSDL(*rect);
            SDL_RenderSetClipRect(m_surfaceContext.GetRenderer(), &currentRect);
        } else {
            SDL_RenderSetClipRect(m_surfaceContext.GetRenderer(), nullptr);
        }
    }

    std::unique_ptr<graphics::ITarget> Graphics::CreateTarget(int width, int height) {
        auto sdlTexture = CreateTextureRef(SDL_CreateTexture(m_surfaceContext.GetRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height));
        auto texture = std::make_shared<Texture>(m_surfaceContext.GetRenderer(), sdlTexture);
        return std::make_unique<Target>(std::move(texture));
    }

    graphics::ITarget* Graphics::GetTarget() {
        return m_currentRenderTarget;
    }

    void Graphics::SetTarget(graphics::ITarget* target) {
        if (target == nullptr) {
            SDL_SetRenderTarget(m_surfaceContext.GetRenderer(), nullptr);
        } else {
            auto* sdlTarget = dynamic_cast<Target*>(target);
            assert(sdlTarget != nullptr && "SetTarget: target is not an SDL Target");
            auto const& sdlTexture = sdlTarget->GetSDLTexture()->GetSDLTexture();
            SDL_SetRenderTarget(m_surfaceContext.GetRenderer(), sdlTexture->GetImpl());
        }
        m_currentRenderTarget = target;
    }

    void Graphics::SetLogicalSize(IntVec2 const& logicalSize) {
        SDL_RenderSetLogicalSize(m_surfaceContext.GetRenderer(), logicalSize.x, logicalSize.y);
    }

    std::unique_ptr<IGraphics> CreateGraphics(SurfaceContext& surfaceContext) {
        return std::make_unique<Graphics>(surfaceContext);
    }
}
