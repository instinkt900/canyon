#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <system_error>
#include <list>
#include <memory>
#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#if !MOTH_GRAPHICS_DISABLE_VULKAN
#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>
#include <GLFW/glfw3.h>
#endif

#if !MOTH_GRAPHICS_DISABLE_SDL
#include <SDL.h>
#include <SDL_rect.h>
#include <SDL_image.h>
#include "graphics/sdl/smart_sdl.hpp"
#endif

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <imgui.h>
#if !MOTH_GRAPHICS_DISABLE_VULKAN
#include <backends/imgui_impl_vulkan.h>
#endif

#undef CreateWindow
#undef min
#undef max

