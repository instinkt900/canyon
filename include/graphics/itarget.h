#pragma once

#include "graphics/iimage.h"
#include "moth_ui/itarget.h"

namespace graphics {
    class ITarget : public moth_ui::ITarget {
    public:
        virtual ~ITarget() = default;

        virtual IImage* GetImage() = 0;
    };
}

