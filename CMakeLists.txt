cmake_minimum_required(VERSION 3.10)

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/version.txt" VERSION_STR)
string(STRIP "${VERSION_STR}" PROJECT_VERSION)

project(canyon VERSION ${PROJECT_VERSION} DESCRIPTION "A basic graphical framework.")

message("Building with CMake version: ${CMAKE_VERSION}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(_CRT_SECURE_NO_WARNINGS IMGUI_DEFINE_MATH_OPERATORS)

include_directories(external/imgui)
include_directories(external/murmurhash.c)

set(SOURCES ${SOURCES}
	external/imgui/backends/imgui_impl_glfw.cpp
	external/imgui/backends/imgui_impl_sdl2.cpp
	external/imgui/backends/imgui_impl_sdlrenderer2.cpp
	external/imgui/backends/imgui_impl_vulkan.cpp
	external/imgui/imgui.cpp
	external/imgui/imgui_draw.cpp
	external/imgui/imgui_tables.cpp
	external/imgui/imgui_widgets.cpp
	src/graphics/font_factory.cpp
	src/graphics/image_factory.cpp
	src/graphics/moth_ui/moth_font_factory.cpp
	src/graphics/moth_ui/moth_image.cpp
	src/graphics/moth_ui/moth_image_factory.cpp
	src/graphics/moth_ui/moth_renderer.cpp
	src/graphics/sdl/sdl_context.cpp
	src/graphics/sdl/sdl_font.cpp
	src/graphics/sdl/sdl_graphics.cpp
	src/graphics/sdl/sdl_image.cpp
	src/graphics/sdl/sdl_surface_context.cpp
	src/graphics/sdl/sdl_texture.cpp
	src/graphics/stb_image.cpp
	src/graphics/stb_image_write.cpp
	src/graphics/stb_rect_pack.cpp
	src/graphics/vulkan/shaders/vulkan_shaders.cpp
	src/graphics/vulkan/vulkan_buffer.cpp
	src/graphics/vulkan/vulkan_command_buffer.cpp 
	src/graphics/vulkan/vulkan_context.cpp
	src/graphics/vulkan/vulkan_fence.cpp
	src/graphics/vulkan/vulkan_font.cpp
	src/graphics/vulkan/vulkan_framebuffer.cpp
	src/graphics/vulkan/vulkan_graphics.cpp
	src/graphics/vulkan/vulkan_image.cpp
	src/graphics/vulkan/vulkan_pipeline.cpp
	src/graphics/vulkan/vulkan_renderpass.cpp
	src/graphics/vulkan/vulkan_shader.cpp
	src/graphics/vulkan/vulkan_surface_context.cpp
	src/graphics/vulkan/vulkan_swapchain.cpp
	src/graphics/vulkan/vulkan_texture.cpp
	src/graphics/vulkan/vulkan_vma.cpp
	src/layers/layer.cpp
	src/layers/layer_stack.cpp
	src/platform/application.cpp
	src/platform/glfw/glfw_events.cpp
	src/platform/glfw/glfw_platform.cpp
	src/platform/glfw/glfw_window.cpp
	src/platform/sdl/sdl_events.cpp
	src/platform/sdl/sdl_platform.cpp
	src/platform/sdl/sdl_window.cpp
	src/platform/window.cpp
	src/widgets/ui_button.cpp
    external/murmurhash.c/murmurhash.c
    src/graphics/sdl/SDL_FontCache.c
)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(glfw3 REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Vulkan REQUIRED)
find_package(vulkan-memory-allocator REQUIRED)
find_package(Freetype REQUIRED)
find_package(spdlog REQUIRED)
find_package(harfbuzz REQUIRED)
find_package(moth_ui REQUIRED)

add_library(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} SDL2::SDL2main)
target_link_libraries(${PROJECT_NAME} SDL2_image::SDL2_image)
target_link_libraries(${PROJECT_NAME} SDL2_ttf::SDL2_ttf-static)
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)
target_link_libraries(${PROJECT_NAME} vulkan-memory-allocator::vulkan-memory-allocator)
target_link_libraries(${PROJECT_NAME} spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} harfbuzz::harfbuzz)
target_link_libraries(${PROJECT_NAME} moth_ui::moth_ui)

if (MSVC)
    set_source_files_properties(src/graphics/sdl/SDL_FontCache.c PROPERTIES COMPILE_FLAGS "/W0")
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ignore:4099")
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/W3 /WX")
else()
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-Wall -Werror")
endif()

target_precompile_headers(${PROJECT_NAME} PRIVATE include/canyon.h)
set_source_files_properties(external/murmurhash.c/murmurhash.c PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
set_source_files_properties(src/graphics/sdl/SDL_FontCache.c PROPERTIES SKIP_PRECOMPILE_HEADERS ON)

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(DIRECTORY "include/" # source directory
        DESTINATION "include" # target directory
        FILES_MATCHING # install only matched files
        PATTERN "*.h" # select header files
        PATTERN "*.hpp" # select header files
)
