#include "common.h"
#include "sdl_texture.h"
#include "sdl_utils.hpp"
#include "smart_sdl.hpp"

#include "imgui.h"

namespace moth_graphics::graphics::sdl {
    Texture::Texture(SDL_Renderer* renderer, SDLTextureRef texture)
        : m_renderer(renderer)
        , m_texture(texture) {
        SDL_QueryTexture(texture->GetImpl(), NULL, NULL, &m_textureDimensions.x, &m_textureDimensions.y);
        SDL_SetTextureScaleMode(m_texture->GetImpl(), m_scaleMode);
    }

    int Texture::GetWidth() const {
        return m_textureDimensions.x;
    }

    int Texture::GetHeight() const {
        return m_textureDimensions.y;
    }

    void Texture::SetFilter(TextureFilter minFilter, TextureFilter magFilter) {
        // SDL2 has a single scale mode rather than separate min/mag filters.
        // Use Nearest if either filter requests it, Linear if both agree.
        SDL_ScaleMode const mode = (minFilter == TextureFilter::Nearest || magFilter == TextureFilter::Nearest)
                                       ? SDL_ScaleModeNearest
                                       : SDL_ScaleModeLinear;

        if (m_scaleMode == mode) {
            return;
        }

        // SDL2's hardware renderer batches draw calls and applies per-texture state
        // (scale mode) at flush time, not at enqueue time. If two nodes share the same
        // SDL_Texture but need different scale modes, the last SetFilter call would win
        // for both. Flushing before a mode change forces any pending batched draws to
        // commit at the old mode before we switch it.
        SDL_RenderFlush(m_renderer);
        SDL_SetTextureScaleMode(m_texture->GetImpl(), mode);
        m_scaleMode = mode;
    }

    void Texture::DrawImGui(IntVec2 const& size, FloatVec2 const& uv0, FloatVec2 const& uv1) const {
        ImGui::Image(m_texture ? m_texture->GetImpl() : nullptr,
                     ImVec2(static_cast<float>(size.x), static_cast<float>(size.y)),
                     ImVec2(uv0.x, uv0.y),
                     ImVec2(uv1.x, uv1.y));
    }

    void Texture::SaveToPNG(std::filesystem::path const& path, IntRect const& sourceRect) {
        SDL_Rect const srcRect = ToSDL(sourceRect);

        SDL_Texture* prevTarget = SDL_GetRenderTarget(m_renderer);
        SDL_SetRenderTarget(m_renderer, m_texture->GetImpl());

        SurfaceRef surface = CreateSurfaceRef(SDL_CreateRGBSurface(0, srcRect.w, srcRect.h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000));
        SDL_RenderReadPixels(m_renderer, &srcRect, surface->format->format, surface->pixels, surface->pitch);
        IMG_SavePNG(surface.get(), path.string().c_str());

        SDL_SetRenderTarget(m_renderer, prevTarget);
    }

    std::unique_ptr<Texture> Texture::FromFile(SDL_Renderer* renderer, std::filesystem::path const& path) {
        auto surface = CreateSurfaceRef(path);
        if (!surface) {
            return nullptr;
        }
        auto texture = CreateTextureRef(renderer, surface);
        if (!texture || texture->GetImpl() == nullptr) {
            return nullptr;
        }
        return std::make_unique<Texture>(renderer, texture);
    }

}

