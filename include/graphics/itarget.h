#pragma once

#include "graphics/iimage.h"

namespace graphics {
    class ITarget {
    public:
        virtual ~ITarget() = default;

        virtual IntVec2 GetDimensions() const = 0;
        virtual IImage* GetImage() = 0;
    };
}

