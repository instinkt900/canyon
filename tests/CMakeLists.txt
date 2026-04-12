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

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME} PRIVATE moth_graphics Catch2::Catch2WithMain)

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(${PROJECT_NAME} PRIVATE /W3 /WX)
    target_link_options(${PROJECT_NAME} PRIVATE /ignore:4099)
endif()

include(CTest)
include(Catch)
catch_discover_tests(${PROJECT_NAME})
