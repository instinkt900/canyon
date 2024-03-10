#pragma once

#include "vector.h"

template <typename T>
inline void to_json(nlohmann::json& j, Vector<T, 2> const& vec) {
    j = nlohmann::json{ { "x", vec.x }, { "y", vec.y } };
}

template <typename T>
inline void from_json(nlohmann::json const& j, Vector<T, 2>& vec) {
    j.at("x").get_to(vec.x);
    j.at("y").get_to(vec.y);
}

template <typename T>
inline void to_json(nlohmann::json& j, Vector<T, 4> const& vec) {
    j = nlohmann::json{ { "x", vec.x }, { "y", vec.y }, { "z", vec.z }, { "w", vec.w } };
}

template <typename T>
inline void from_json(nlohmann::json const& j, Vector<T, 4>& vec) {
    j.at("x").get_to(vec.x);
    j.at("y").get_to(vec.y);
    j.at("z").get_to(vec.z);
    j.at("w").get_to(vec.w);
}

