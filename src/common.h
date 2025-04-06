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

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>
#include <GLFW/glfw3.h>

#include <SDL.h>
#include <SDL_rect.h>
#include <SDL_image.h>
#include "graphics/sdl/smart_sdl.hpp"

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

#undef CreateWindow
#undef min
#undef max

