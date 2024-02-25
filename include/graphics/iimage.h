#pragma once

#include "utils/vector.h"

namespace graphics {
    class IImage {
    public:
        virtual ~IImage() = default;

        virtual int GetWidth() const = 0;
        virtual int GetHeight() const = 0;
        virtual IntVec2 GetDimensions() const = 0;

        virtual void ImGui(IntVec2 const& size, FloatVec2 const& uv0 = { 0, 0 }, FloatVec2 const& uv1 = { 1, 1 }) const = 0;
    };
}

