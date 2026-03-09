#pragma once

#include "canyon/graphics/texture_filter.h"

namespace canyon::graphics {
    class ITexture {
    public:
        virtual ~ITexture() = default;

        virtual int GetWidth() const = 0;
        virtual int GetHeight() const = 0;
        virtual void SetFilter(TextureFilter minFilter, TextureFilter magFilter) = 0;
    };
}
