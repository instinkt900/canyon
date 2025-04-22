#pragma once

#include "canyon/utils/vector.h"

namespace canyon::graphics {
    class IImage {
    public:
        virtual ~IImage() = default;

        virtual int GetWidth() const = 0;
        virtual int GetHeight() const = 0;

        virtual void ImGui(canyon::IntVec2 const& size, canyon::FloatVec2 const& uv0 = { 0, 0 }, canyon::FloatVec2 const& uv1 = { 1, 1 }) const = 0;
    };
}
