#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <stack>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <chrono>
#include <array>
#include <cassert>
#include <fstream>
#include <list>

#if !CANYON_DISABLE_VULKAN
#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>
#include <GLFW/glfw3.h>
#endif

#if !CANYON_DISABLE_SDL
#include <SDL.h>
#include <SDL_rect.h>
#include <SDL_image.h>
#include "canyon/graphics/sdl/smart_sdl.hpp"
#endif

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <imgui.h>
#if !CANYON_DISABLE_VULKAN
#include <backends/imgui_impl_vulkan.h>
#endif

#undef CreateWindow
#undef min
#undef max

