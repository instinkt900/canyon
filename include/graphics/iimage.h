#pragma once

#include "moth_ui/iimage.h"

namespace graphics {
    class IImage : public moth_ui::IImage {
    public:
        virtual ~IImage() = default;

        virtual int GetWidth() const = 0;
        virtual int GetHeight() const = 0;
    };
}

