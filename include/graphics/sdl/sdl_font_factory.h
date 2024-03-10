#pragma once

#include "graphics/font_factory.h"
#include "graphics/ifont.h"

namespace graphics::sdl {
    class FontFactory : public graphics::FontFactory {
    public:
        FontFactory(SDL_Renderer& renderer);
        virtual ~FontFactory() = default;

        std::shared_ptr<graphics::IFont> GetFont(char const* name, int size) override;

    private:
        SDL_Renderer& m_renderer;
    };
}
