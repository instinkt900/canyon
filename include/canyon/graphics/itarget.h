#pragma once

#include "canyon/graphics/iimage.h"

namespace canyon::graphics {
    class ITarget : public IImage {
    public:
        virtual ~ITarget() = default;
    };
}
