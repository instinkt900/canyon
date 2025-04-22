#pragma once

#include "canyon/graphics/iimage.h"

namespace canyon::graphics {
    class ITarget {
    public:
        virtual ~ITarget() = default;

        virtual IImage* GetImage() = 0;
    };
}
