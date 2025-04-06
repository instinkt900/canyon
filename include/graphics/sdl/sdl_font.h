#pragma once

#include "graphics/ifont.h"
#include "smart_sdl.hpp"

#include <SDL_render.h>
#include <memory>
#include <filesystem>

namespace graphics::sdl {
    class Font : public IFont {
    public:
        explicit Font(CachedFontRef fontObj);
        virtual ~Font() = default;

        CachedFontRef GetFontObj() const {
            return m_fontObj;
        }

        static std::unique_ptr<IFont> Load(SDL_Renderer& renderer, const std::filesystem::path& path, int size);

    private:
        CachedFontRef m_fontObj;
    };
}
