cmake_minimum_required(VERSION 3.10)

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/version.txt" VERSION_STR)
string(STRIP "${VERSION_STR}" PROJECT_VERSION)

project(moth_graphics VERSION ${PROJECT_VERSION} DESCRIPTION "A basic graphical framework.")

message("Building with CMake version: ${CMAKE_VERSION}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(MOTH_GRAPHICS_DISABLE_VULKAN "Disable the Vulkan/GLFW backend" OFF)
option(MOTH_GRAPHICS_DISABLE_SDL "Disable the SDL2 backend" OFF)

if(MOTH_GRAPHICS_DISABLE_VULKAN AND MOTH_GRAPHICS_DISABLE_SDL)
    message(FATAL_ERROR "At least one backend must be enabled (MOTH_GRAPHICS_DISABLE_VULKAN and MOTH_GRAPHICS_DISABLE_SDL cannot both be ON).")
endif()

# ---- Always-compiled sources ------------------------------------------------
set(SOURCES
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    src/graphics/spritesheet.cpp
    src/graphics/spritesheet_factory.cpp
    src/graphics/font_factory.cpp
    src/graphics/image_factory.cpp
    src/graphics/moth_ui/moth_flipbook.cpp
    src/graphics/moth_ui/moth_flipbook_factory.cpp
    src/graphics/moth_ui/moth_font_factory.cpp
    src/graphics/moth_ui/moth_image.cpp
    src/graphics/moth_ui/moth_image_factory.cpp
    src/graphics/moth_ui/moth_renderer.cpp
    src/graphics/stb_image.cpp
    src/graphics/stb_image_write.cpp
    src/graphics/stb_rect_pack.cpp
    src/platform/application.cpp
    src/platform/window.cpp
    external/murmurhash.c/murmurhash.c
)

# ---- SDL backend sources -----------------------------------------------------
if(NOT MOTH_GRAPHICS_DISABLE_SDL)
    list(APPEND SOURCES
        external/imgui/backends/imgui_impl_sdl2.cpp
        external/imgui/backends/imgui_impl_sdlrenderer2.cpp
        src/graphics/sdl/sdl_context.cpp
        src/graphics/sdl/sdl_font.cpp
        src/graphics/sdl/sdl_graphics.cpp
        src/graphics/sdl/sdl_image.cpp
        src/graphics/sdl/sdl_asset_context.cpp
        src/graphics/sdl/sdl_surface_context.cpp
        src/graphics/sdl/sdl_texture.cpp
        src/graphics/sdl/SDL_FontCache.c
        src/platform/sdl/sdl_events.cpp
        src/platform/sdl/sdl_platform.cpp
        src/platform/sdl/sdl_window.cpp
    )
endif()

# ---- Vulkan backend sources --------------------------------------------------
if(NOT MOTH_GRAPHICS_DISABLE_VULKAN)
    list(APPEND SOURCES
        external/imgui/backends/imgui_impl_glfw.cpp
        external/imgui/backends/imgui_impl_vulkan.cpp
        src/graphics/vulkan/shaders/vulkan_shaders.cpp
        src/graphics/vulkan/vulkan_buffer.cpp
        src/graphics/vulkan/vulkan_command_buffer.cpp
        src/graphics/vulkan/vulkan_context.cpp
        src/graphics/vulkan/vulkan_fence.cpp
        src/graphics/vulkan/vulkan_font.cpp
        src/graphics/vulkan/vulkan_framebuffer.cpp
        src/graphics/vulkan/vulkan_graphics.cpp
        src/graphics/vulkan/vulkan_graphics_frame.cpp
        src/graphics/vulkan/vulkan_graphics_imgui.cpp
        src/graphics/vulkan/vulkan_graphics_pipeline.cpp
        src/graphics/vulkan/vulkan_image.cpp
        src/graphics/vulkan/vulkan_pipeline.cpp
        src/graphics/vulkan/vulkan_renderpass.cpp
        src/graphics/vulkan/vulkan_shader.cpp
        src/graphics/vulkan/vulkan_asset_context.cpp
        src/graphics/vulkan/vulkan_surface_context.cpp
        src/graphics/vulkan/vulkan_swapchain.cpp
        src/graphics/vulkan/vulkan_texture.cpp
        src/graphics/vulkan/vulkan_vma.cpp
        src/platform/glfw/glfw_events.cpp
        src/platform/glfw/glfw_platform.cpp
        src/platform/glfw/glfw_window.cpp
    )
endif()

# ---- Dependencies ------------------------------------------------------------

# SDL2/SDL_image/SDL_ttf/GLFW must come from the system on Linux to avoid
# runtime symbol conflicts with GTK3/GDK-Pixbuf (used by NFD's file dialog).
# Conan-built copies statically embed libpng, libjpeg, etc. which clash with
# the system versions loaded by GTK3 at runtime.
if(NOT MOTH_GRAPHICS_DISABLE_SDL)
    if(WIN32)
        find_package(SDL2 REQUIRED)
        find_package(SDL2_image REQUIRED)
        find_package(SDL2_ttf REQUIRED)
        set(MOTH_GRAPHICS_SDL2_LINK SDL2::SDL2)
        set(MOTH_GRAPHICS_SDL2_IMAGE_LINK SDL2_image::SDL2_image)
        set(MOTH_GRAPHICS_SDL2_TTF_LINK SDL2_ttf::SDL2_ttf-static)
    else()
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)
        pkg_check_modules(SDL2_image REQUIRED IMPORTED_TARGET SDL2_image)
        pkg_check_modules(SDL2_ttf REQUIRED IMPORTED_TARGET SDL2_ttf)
        set(MOTH_GRAPHICS_SDL2_LINK PkgConfig::SDL2)
        set(MOTH_GRAPHICS_SDL2_IMAGE_LINK PkgConfig::SDL2_image)
        set(MOTH_GRAPHICS_SDL2_TTF_LINK PkgConfig::SDL2_ttf)
    endif()
endif()

if(NOT MOTH_GRAPHICS_DISABLE_VULKAN)
    if(WIN32)
        find_package(glfw3 REQUIRED)
        find_package(Freetype REQUIRED)
        find_package(harfbuzz REQUIRED)
        set(MOTH_GRAPHICS_GLFW_LINK glfw)
    else()
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(GLFW3 REQUIRED IMPORTED_TARGET glfw3)
        set(MOTH_GRAPHICS_GLFW_LINK PkgConfig::GLFW3)
        find_package(Freetype REQUIRED)
        find_package(harfbuzz REQUIRED)
    endif()
    find_package(Vulkan REQUIRED)
    find_package(VulkanMemoryAllocator REQUIRED)
endif()

find_package(spdlog REQUIRED)
find_package(moth_ui REQUIRED)

# ---- Target ------------------------------------------------------------------

add_library(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
target_compile_definitions(${PROJECT_NAME} PUBLIC IMGUI_DEFINE_MATH_OPERATORS)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external/imgui>
    $<INSTALL_INTERFACE:external/imgui>
)
target_include_directories(${PROJECT_NAME} PRIVATE external/murmurhash.c)
target_include_directories(${PROJECT_NAME} PRIVATE external/stb)
target_include_directories(${PROJECT_NAME} PRIVATE src)

if(NOT MOTH_GRAPHICS_DISABLE_SDL)
    target_compile_definitions(${PROJECT_NAME} PUBLIC MOTH_GRAPHICS_DISABLE_SDL=0)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${MOTH_GRAPHICS_SDL2_LINK})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${MOTH_GRAPHICS_SDL2_IMAGE_LINK})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${MOTH_GRAPHICS_SDL2_TTF_LINK})
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC MOTH_GRAPHICS_DISABLE_SDL=1)
    set_source_files_properties(src/graphics/sdl/SDL_FontCache.c PROPERTIES HEADER_FILE_ONLY TRUE)
endif()

if(NOT MOTH_GRAPHICS_DISABLE_VULKAN)
    target_compile_definitions(${PROJECT_NAME} PUBLIC MOTH_GRAPHICS_DISABLE_VULKAN=0)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${MOTH_GRAPHICS_GLFW_LINK})
    target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan::Vulkan)
    target_link_libraries(${PROJECT_NAME} PUBLIC GPUOpen::VulkanMemoryAllocator)
    target_link_libraries(${PROJECT_NAME} PUBLIC Freetype::Freetype)
    target_link_libraries(${PROJECT_NAME} PUBLIC harfbuzz::harfbuzz)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC MOTH_GRAPHICS_DISABLE_VULKAN=1)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} PUBLIC moth_ui::moth_ui)

if(MSVC)
    if(NOT MOTH_GRAPHICS_DISABLE_SDL)
        set_source_files_properties(src/graphics/sdl/SDL_FontCache.c PROPERTIES COMPILE_FLAGS "/W0")
    endif()
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ignore:4099")
    set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/W3 /WX")
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS MOTH_GRAPHICS_BUILD)
    if(BUILD_SHARED_LIBS)
        target_compile_definitions(${PROJECT_NAME} PRIVATE MOTH_GRAPHICS_BUILD_SHARED)
    endif()
else()
    if(NOT MOTH_GRAPHICS_DISABLE_SDL)
        set_source_files_properties(src/graphics/sdl/SDL_FontCache.c PROPERTIES COMPILE_FLAGS "-w")
    endif()
    set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-Wall -Werror")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR DEFINED ENV{CI})
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)
    if(CLANG_TIDY_EXE)
        # --extra-arg=-Wno-ignored-gch: clang-tidy uses Clang, which cannot read
        # GCC-format PCH files. Suppressing the warning entirely (not just -Wno-error)
        # prevents clang-tidy's WarningsAsErrors from re-promoting it to an error.
        set_target_properties(${PROJECT_NAME} PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_EXE};--extra-arg=-Wno-ignored-gch")
        # Third-party source files — skip all linting (requires CMake 3.27+)
        set_source_files_properties(
            src/graphics/sdl/SDL_FontCache.c
            external/imgui/imgui.cpp
            external/imgui/imgui_draw.cpp
            external/imgui/imgui_tables.cpp
            external/imgui/imgui_widgets.cpp
            external/imgui/backends/imgui_impl_sdl2.cpp
            external/imgui/backends/imgui_impl_sdlrenderer2.cpp
            external/imgui/backends/imgui_impl_glfw.cpp
            external/imgui/backends/imgui_impl_vulkan.cpp
            external/murmurhash.c/murmurhash.c
            src/graphics/vulkan/shaders/vulkan_shaders.cpp
            PROPERTIES SKIP_LINTING ON
        )
    elseif(DEFINED ENV{CI})
        message(FATAL_ERROR "clang-tidy not found but required in CI")
    else()
        message(STATUS "clang-tidy not found, skipping")
    endif()
endif()

target_precompile_headers(${PROJECT_NAME} PRIVATE src/common.h)
set_source_files_properties(
    external/murmurhash.c/murmurhash.c
    src/graphics/sdl/SDL_FontCache.c
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_sdl2.cpp
    external/imgui/backends/imgui_impl_sdlrenderer2.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_vulkan.cpp
    src/graphics/vulkan/shaders/vulkan_shaders.cpp
    PROPERTIES SKIP_PRECOMPILE_HEADERS ON
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)
install(DIRECTORY "include/"
    DESTINATION "include"
    FILES_MATCHING PATTERN "*.h"
)
install(DIRECTORY "external/imgui/"
    DESTINATION "external/imgui"
    FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
)
