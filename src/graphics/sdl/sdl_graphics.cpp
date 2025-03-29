#include "canyon.h"
#include "graphics/sdl/sdl_graphics.h"
#include "graphics/sdl/sdl_context.h"
#include "graphics/sdl/sdl_font.h"
#include "graphics/sdl/sdl_image.h"
#include "graphics/sdl/sdl_utils.h"
#include "../utils.h"

namespace graphics::sdl {
    Graphics::Graphics(SurfaceContext& context)
        : m_surfaceContext(context)
        , m_drawColor(graphics::BasicColors::White) {
    }

    void Graphics::SetBlendMode(graphics::BlendMode mode) {
        SDL_SetRenderDrawBlendMode(m_surfaceContext.GetRenderer(), ToSDL(mode));
        m_blendMode = mode;
    }

    // void Graphics::SetBlendMode(std::shared_ptr<graphics::IImage> target, graphics::BlendMode mode) {
    //     auto sdlImage = std::dynamic_pointer_cast<Image>(target);
    //     auto sdlTexture = sdlImage->GetTexture();
    //     SDL_SetTextureBlendMode(sdlTexture->GetImpl(), ToSDL(mode));
    // }

    // void SDLGraphics::SetColorMod(std::shared_ptr<graphics::IImage> target, graphics::Color const& color) {
    //     auto sdlImage = std::dynamic_pointer_cast<Image>(target);
    //     auto sdlTexture = sdlImage->GetTexture();
    //     ColorComponents components(color);
    //     SDL_SetTextureColorMod(sdlTexture->GetImpl(), components.r, components.g, components.b);
    //     SDL_SetTextureAlphaMod(sdlTexture->GetImpl(), components.a);
    // }

    void Graphics::SetColor(graphics::Color const& color) {
        ColorComponents components(color);
        SDL_SetRenderDrawColor(m_surfaceContext.GetRenderer(), components.r, components.g, components.b, components.a);
        m_drawColor = color;
    }

    void Graphics::Clear() {
        SDL_RenderClear(m_surfaceContext.GetRenderer());
    }

    void Graphics::DrawImage(graphics::IImage& image, IntRect const& destRect, IntRect const* sourceRect) {
        auto& sdlImage = dynamic_cast<Image&>(image);
        auto sdlTexture = sdlImage.GetTexture();
        // auto const& textureSourceRect = sdlImage.GetSourceRect();

        ColorComponents const components{ m_drawColor };
        SDL_SetTextureBlendMode(sdlTexture->GetSDLTexture()->GetImpl(), ToSDL(m_blendMode));
        SDL_SetTextureColorMod(sdlTexture->GetSDLTexture()->GetImpl(), components.r, components.g, components.b);
        SDL_SetTextureAlphaMod(sdlTexture->GetSDLTexture()->GetImpl(), components.a);

        SDL_Rect sdlSrcRect = ToSDL(*sourceRect);
        SDL_Rect sdlDstRect = ToSDL(destRect);

        // if (sourceRect) {
        //     sdlSrcRect.x += sourceRect->x();
        //     sdlSrcRect.y += sourceRect->y();
        // }

        SDL_RenderCopy(m_surfaceContext.GetRenderer(), sdlTexture->GetSDLTexture()->GetImpl(), &sdlSrcRect, &sdlDstRect);
    }

    void Graphics::DrawImageTiled(graphics::IImage& image, IntRect const& destRect, IntRect const* sourceRect, float scale) {
        auto& sdlImage = dynamic_cast<Image&>(image);
        auto sdlTexture = sdlImage.GetTexture();
        auto const& textureSourceRect = sdlImage.GetSourceRect();

        ColorComponents const components{ m_drawColor };
        SDL_SetTextureBlendMode(sdlTexture->GetSDLTexture()->GetImpl(), ToSDL(m_blendMode));
        SDL_SetTextureColorMod(sdlTexture->GetSDLTexture()->GetImpl(), components.r, components.g, components.b);
        SDL_SetTextureAlphaMod(sdlTexture->GetSDLTexture()->GetImpl(), components.a);

        SDL_Rect sdlSrcRect = ToSDL(textureSourceRect);
        if (sourceRect) {
            sdlSrcRect.x += sourceRect->x();
            sdlSrcRect.y += sourceRect->y();
        }

        auto const imageWidth = static_cast<int>(image.GetWidth() * scale);
        auto const imageHeight = static_cast<int>(image.GetHeight() * scale);
        auto const sdlTotalDestRect{ ToSDL(destRect) };
        SDL_RenderSetClipRect(m_surfaceContext.GetRenderer(), &sdlTotalDestRect);
        for (auto y = destRect.topLeft.y; y < destRect.bottomRight.y; y += imageHeight) {
            for (auto x = destRect.topLeft.x; x < destRect.bottomRight.x; x += imageWidth) {
                SDL_Rect sdlDstRect{ x, y, imageWidth, imageHeight };
                SDL_RenderCopy(m_surfaceContext.GetRenderer(), sdlTexture->GetSDLTexture()->GetImpl(), &sdlSrcRect, &sdlDstRect);
            }
        }
        SDL_RenderSetClipRect(m_surfaceContext.GetRenderer(), nullptr);
    }

    void Graphics::DrawToPNG(std::filesystem::path const& path) {
        Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0xff000000;
        gmask = 0x00ff0000;
        bmask = 0x0000ff00;
        amask = 0x000000ff;
#else
        rmask = 0x000000ff;
        gmask = 0x0000ff00;
        bmask = 0x00ff0000;
        amask = 0xff000000;
#endif

        int width;
        int height;
        SDL_GetRendererOutputSize(m_surfaceContext.GetRenderer(), &width, &height);

        SurfaceRef surface = CreateSurfaceRef(SDL_CreateRGBSurface(0, width, height, 32, rmask, gmask, bmask, amask));
        SDL_RenderReadPixels(m_surfaceContext.GetRenderer(), nullptr, surface->format->format, surface->pixels, surface->pitch);
        IMG_SavePNG(surface.get(), path.string().c_str());
    }

    void Graphics::DrawRectF(FloatRect const& rect) {
        auto const sdlRectF = ToSDL(rect);
        SDL_RenderDrawRectF(m_surfaceContext.GetRenderer(), &sdlRectF);
    }

    void Graphics::DrawFillRectF(FloatRect const& rect) {
        auto const sdlRectF = ToSDL(rect);
        SDL_RenderFillRectF(m_surfaceContext.GetRenderer(), &sdlRectF);
    }

    void Graphics::DrawLineF(FloatVec2 const& p0, FloatVec2 const& p1) {
        SDL_RenderDrawLineF(m_surfaceContext.GetRenderer(), p0.x, p0.y, p1.x, p1.y);
    }

    void Graphics::DrawText(std::string const& text, graphics::IFont& font, graphics::TextHorizAlignment horizontalAlignment, graphics::TextVertAlignment verticalAlignment, IntRect const& destRect) {
        auto const fcFont = static_cast<Font&>(font).GetFontObj();

        auto const destWidth = destRect.bottomRight.x - destRect.topLeft.x;
        auto const destHeight = destRect.bottomRight.y - destRect.topLeft.y;
        auto const textHeight = FC_GetColumnHeight(fcFont.get(), destWidth, "%s", text.c_str());

        auto x = static_cast<float>(destRect.topLeft.x);
        switch (horizontalAlignment) {
        case graphics::TextHorizAlignment::Left:
            break;
        case graphics::TextHorizAlignment::Center:
            x = x + destWidth / 2.0f;
            break;
        case graphics::TextHorizAlignment::Right:
            x = x + destWidth;
            break;
        }

        auto y = static_cast<float>(destRect.topLeft.y);
        switch (verticalAlignment) {
        case graphics::TextVertAlignment::Top:
            break;
        case graphics::TextVertAlignment::Middle:
            y = y + (destHeight - textHeight) / 2.0f;
            break;
        case graphics::TextVertAlignment::Bottom:
            y = y + destHeight - textHeight;
            break;
        }

        FC_Effect effect;
        effect.alignment = ToSDL(horizontalAlignment);
        effect.color = ToSDL(m_drawColor);
        // effect.color = { 0xff, 0xff, 0xff, 0xff };
        effect.scale.x = 1.0f;
        effect.scale.y = 1.0f;

        FC_DrawColumnEffect(fcFont.get(), m_surfaceContext.GetRenderer(), x, y, destWidth, effect, "%s", text.c_str());
    }

    void Graphics::SetClip(IntRect const* rect) {
        if (rect) {
            auto const currentRect = ToSDL(*rect);
            SDL_RenderSetClipRect(m_surfaceContext.GetRenderer(), &currentRect);
        } else {
            SDL_RenderSetClipRect(m_surfaceContext.GetRenderer(), nullptr);
        }
    }

    std::unique_ptr<graphics::ITarget> Graphics::CreateTarget(int width, int height) {
        auto sdlTexture = CreateTextureRef(SDL_CreateTexture(m_surfaceContext.GetRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height));
        auto texture = std::make_shared<Texture>(sdlTexture);
        IntRect const sourceRect{ { 0, 0 }, { width, height } };
        return std::make_unique<Image>(texture, sourceRect);
    }

    graphics::ITarget* Graphics::GetTarget() {
        return m_currentRenderTarget;
    }

    void Graphics::SetTarget(graphics::ITarget* target) {
        if (!target) {
            SDL_SetRenderTarget(m_surfaceContext.GetRenderer(), nullptr);
        } else {
            auto sdlImage = dynamic_cast<Texture*>(target);
            auto sdlTexture = sdlImage->GetSDLTexture();
            SDL_SetRenderTarget(m_surfaceContext.GetRenderer(), sdlTexture->GetImpl());
        }
        m_currentRenderTarget = target;
    }

    void Graphics::SetLogicalSize(IntVec2 const& logicalSize) {
        SDL_RenderSetLogicalSize(m_surfaceContext.GetRenderer(), logicalSize.x, logicalSize.y);
    }
}
