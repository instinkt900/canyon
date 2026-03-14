#include "common.h"
#include "canyon/graphics/sdl/sdl_graphics.h"
#include "canyon/graphics/sdl/sdl_font.h"
#include "canyon/graphics/sdl/sdl_image.h"
#include "canyon/graphics/sdl/sdl_utils.hpp"
#include "canyon/platform/sdl/sdl_window.h"
#include "../utils.h"

#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"

namespace canyon::graphics::sdl {
    Graphics::Graphics(SurfaceContext& context)
        : m_surfaceContext(context)
        , m_drawColor(graphics::BasicColors::White) {
    }

    Graphics::~Graphics() {
        if (m_imguiWindow != nullptr) {
            ImGui_ImplSDLRenderer2_Shutdown();
            ImGui_ImplSDL2_Shutdown();
            ImGui::DestroyContext();
        }
    }

    void Graphics::InitImgui(canyon::platform::Window const& window) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        auto& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        ImGui::StyleColorsDark();

        const auto& sdlWindow = dynamic_cast<canyon::platform::sdl::Window const&>(window);
        m_imguiWindow = sdlWindow.GetSDLWindow();
        ImGui_ImplSDL2_InitForSDLRenderer(sdlWindow.GetSDLWindow(), sdlWindow.GetSDLRenderer());
        ImGui_ImplSDLRenderer2_Init(sdlWindow.GetSDLRenderer());
    }

    void Graphics::Begin() {
        if (m_imguiWindow != nullptr) {
            ImGui_ImplSDLRenderer2_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();
        }
    }

    void Graphics::End() {
        if (m_imguiWindow != nullptr) {
            ImGui::Render();
            ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
        }
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

    void Graphics::DrawImage(graphics::IImage& image, IntRect const& destRect, IntRect const* sourceRect, float rotation) {
        auto& sdlImage = dynamic_cast<Image&>(image);
        auto sdlTexture = std::dynamic_pointer_cast<Texture>(sdlImage.GetTexture());
        if (!sdlTexture) {
            return;
        }
        auto const& textureSourceRect = sdlImage.GetSourceRect();

        ColorComponents const components{ m_drawColor };
        SDL_SetTextureBlendMode(sdlTexture->GetSDLTexture()->GetImpl(), ToSDL(m_blendMode));
        SDL_SetTextureColorMod(sdlTexture->GetSDLTexture()->GetImpl(), components.r, components.g, components.b);
        SDL_SetTextureAlphaMod(sdlTexture->GetSDLTexture()->GetImpl(), components.a);

        SDL_Rect sdlSrcRect = ToSDL(sourceRect != nullptr ? *sourceRect : textureSourceRect);
        SDL_Rect sdlDstRect = ToSDL(destRect);

        // Negative src dimensions mean the caller wants a mirrored draw.
        // SDL doesn't support negative-dimension rects; normalise and flip instead.
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

        SDL_RenderCopyEx(m_surfaceContext.GetRenderer(),
                         sdlTexture->GetSDLTexture()->GetImpl(),
                         &sdlSrcRect,
                         &sdlDstRect,
                         rotation,
                         nullptr,
                         flip);
    }

    void Graphics::DrawImageTiled(graphics::IImage& image, IntRect const& destRect, IntRect const* sourceRect, float scale) {
        auto& sdlImage = dynamic_cast<Image&>(image);
        auto sdlTexture = std::dynamic_pointer_cast<Texture>(sdlImage.GetTexture());
        auto const& textureSourceRect = sdlImage.GetSourceRect();

        ColorComponents const components{ m_drawColor };
        SDL_SetTextureBlendMode(sdlTexture->GetSDLTexture()->GetImpl(), ToSDL(m_blendMode));
        SDL_SetTextureColorMod(sdlTexture->GetSDLTexture()->GetImpl(), components.r, components.g, components.b);
        SDL_SetTextureAlphaMod(sdlTexture->GetSDLTexture()->GetImpl(), components.a);

        SDL_Rect sdlSrcRect = ToSDL(sourceRect != nullptr ? *sourceRect : textureSourceRect);

        auto const imageWidth = static_cast<int>(static_cast<float>(image.GetWidth()) * scale);
        auto const imageHeight = static_cast<int>(static_cast<float>(image.GetHeight()) * scale);
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

    void Graphics::DrawToPNG(IImage& image, std::filesystem::path const& path) {
        auto& sdlImage = dynamic_cast<Image&>(image);
        auto sdlTexture = std::dynamic_pointer_cast<Texture>(sdlImage.GetTexture());
        SDL_Texture* tex = sdlTexture->GetSDLTexture()->GetImpl();

        int width = image.GetWidth();
        int height = image.GetHeight();

        SDL_Texture* prevTarget = SDL_GetRenderTarget(m_surfaceContext.GetRenderer());
        SDL_SetRenderTarget(m_surfaceContext.GetRenderer(), tex);

        SurfaceRef surface = CreateSurfaceRef(SDL_CreateRGBSurface(0, width, height, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000));
        SDL_RenderReadPixels(m_surfaceContext.GetRenderer(), nullptr, surface->format->format, surface->pixels, surface->pitch);
        IMG_SavePNG(surface.get(), path.string().c_str());

        SDL_SetRenderTarget(m_surfaceContext.GetRenderer(), prevTarget);
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

    void Graphics::DrawText(std::string const& text, graphics::IFont& font, IntRect const& destRect, graphics::TextHorizAlignment horizontalAlignment, graphics::TextVertAlignment verticalAlignment) {
        auto const fcFont = dynamic_cast<Font&>(font).GetFontObj();

        auto const destWidth = destRect.bottomRight.x - destRect.topLeft.x;
        auto const destHeight = destRect.bottomRight.y - destRect.topLeft.y;
        auto const textHeight = FC_GetColumnHeight(fcFont.get(), destWidth, "%s", text.c_str()); // NOLINT(cppcoreguidelines-pro-type-vararg)

        auto x = static_cast<float>(destRect.topLeft.x);
        switch (horizontalAlignment) {
        case graphics::TextHorizAlignment::Left:
            break;
        case graphics::TextHorizAlignment::Center:
            x = x + static_cast<float>(destWidth) / 2.0f;
            break;
        case graphics::TextHorizAlignment::Right:
            x = x + static_cast<float>(destWidth);
            break;
        }

        auto y = static_cast<float>(destRect.topLeft.y);
        switch (verticalAlignment) {
        case graphics::TextVertAlignment::Top:
            break;
        case graphics::TextVertAlignment::Middle:
            y = y + (static_cast<float>(destHeight) - static_cast<float>(textHeight)) / 2.0f;
            break;
        case graphics::TextVertAlignment::Bottom:
            y = y + static_cast<float>(destHeight) - static_cast<float>(textHeight);
            break;
        }

        FC_Effect effect;
        effect.alignment = ToSDL(horizontalAlignment);
        effect.color = ToSDL(m_drawColor);
        effect.scale.x = 1.0f;
        effect.scale.y = 1.0f;

        FC_DrawColumnEffect(fcFont.get(), m_surfaceContext.GetRenderer(), x, y, destWidth, effect, "%s", text.c_str()); // NOLINT(cppcoreguidelines-pro-type-vararg)
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
        auto texture = std::make_shared<Texture>(sdlTexture);
        IntRect const sourceRect{ { 0, 0 }, { width, height } };
        return std::make_unique<Image>(texture, sourceRect);
    }

    graphics::ITarget* Graphics::GetTarget() {
        return m_currentRenderTarget;
    }

    void Graphics::SetTarget(graphics::ITarget* target) {
        if (target == nullptr) {
            SDL_SetRenderTarget(m_surfaceContext.GetRenderer(), nullptr);
        } else {
            auto* image = dynamic_cast<Image*>(target);
            assert(image && "SetTarget: target is not an Image");
            auto sdlImage = std::dynamic_pointer_cast<Texture>(image->GetTexture());
            assert(sdlImage && "SetTarget: texture is not an SDL Texture");
            auto sdlTexture = sdlImage->GetSDLTexture();
            SDL_SetRenderTarget(m_surfaceContext.GetRenderer(), sdlTexture->GetImpl());
        }
        m_currentRenderTarget = target;
    }

    void Graphics::SetLogicalSize(IntVec2 const& logicalSize) {
        SDL_RenderSetLogicalSize(m_surfaceContext.GetRenderer(), logicalSize.x, logicalSize.y);
    }
}
