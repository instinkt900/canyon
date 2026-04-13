cmake_minimum_required(VERSION 3.15)

project(moth_graphics_tests VERSION 1.0 DESCRIPTION "Unit tests for moth_graphics")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Catch2 3 REQUIRED)

add_subdirectory(.. moth_graphics)

set(SOURCES
    src/test_spritesheet.cpp
    src/test_sprite.cpp
    src/test_ticker.cpp
    src/test_event_emitter.cpp
    # API surface tests — compile-time only, no runtime assertions
    src/api_surface_umbrella.cpp
    src/api_surface_enums.cpp
    src/api_surface_graphics.cpp
    src/api_surface_sprite.cpp
    src/api_surface_events.cpp
    src/api_surface_platform.cpp
    src/api_surface_moth_bridge.cpp
)

if(WIN32)
    # SDL2main.lib (linked via SDL2::SDL2main) provides WinMain which calls SDL_main.
    # Catch2WithMain owns main and does not define SDL_main.
    # This stub satisfies the unresolved SDL_main symbol by forwarding to main.
    list(APPEND SOURCES src/sdl_main_stub.cpp)
endif()

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME} PRIVATE moth_graphics Catch2::Catch2WithMain)

if(MSVC)
    # SDL_MAIN_HANDLED: prevent SDL2 from redefining main→SDL_main; Catch2 owns main.
    # /utf-8: interpret source files as UTF-8 so Unicode chars in headers (e.g. moth_ui_format.h) don't trigger C4566.
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS SDL_MAIN_HANDLED)
    target_compile_options(${PROJECT_NAME} PRIVATE /W3 /WX /utf-8)
    target_link_options(${PROJECT_NAME} PRIVATE /ignore:4099)
endif()

include(CTest)
include(Catch)
catch_discover_tests(${PROJECT_NAME})
