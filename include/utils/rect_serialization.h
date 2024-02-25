#pragma once

#include "rect.h"
#include "vector_serialization.h"

template <typename T>
inline void to_json(nlohmann::json& j, Rect<T> const& rect) {
    j = nlohmann::json{ { "topLeft", rect.topLeft }, { "bottomRight", rect.bottomRight } };
}

template <typename T>
inline void from_json(nlohmann::json const& j, Rect<T>& rect) {
    j.at("topLeft").get_to(rect.topLeft);
    j.at("bottomRight").get_to(rect.bottomRight);
}

