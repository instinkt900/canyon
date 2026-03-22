#pragma once

#include "moth_graphics/graphics/ifont.h"
#include "moth_graphics/graphics/sdl/smart_sdl.hpp"

#include <SDL_render.h>
#include <memory>
#include <filesystem>

namespace moth_graphics::graphics::sdl {
    class Font : public IFont {
    public:
        explicit Font(CachedFontRef fontObj);
        ~Font() override = default;

        CachedFontRef GetFontObj() const {
            return m_fontObj;
        }

        static std::unique_ptr<IFont> Load(SDL_Renderer& renderer, const std::filesystem::path& path, int size);

    private:
        CachedFontRef m_fontObj;
    };
}
