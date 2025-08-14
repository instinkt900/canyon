#pragma once

#include <cmath>
#include <moth_ui/utils/interp.h>

namespace canyon {
    template<typename T, typename F>
    inline T Lerp(T const& min, T const& max, F const factor) {
        return min + (max - min) * factor;
    }

    template<typename T>
    inline T Radians(T const degrees) {
        return degrees * (M_PI / 180.0f);
    }

    template<typename T>
    inline T Degrees(T const radians) {
        return radians * (180.0f / M_PI);
    }

    using moth_ui::InterpType;
    using moth_ui::Interp;
    using moth_ui::F_PI;
}

