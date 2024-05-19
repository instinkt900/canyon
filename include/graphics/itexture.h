#pragma once

#include "utils/vector.h"

namespace graphics {
    class ITexture {
    public:
        virtual ~ITexture() = default;

        virtual int GetWidth() const = 0;
        virtual int GetHeight() const = 0;
    };
}
