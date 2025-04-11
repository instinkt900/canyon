cmake_minimum_required(VERSION 3.10)

project(canyon-example VERSION 1.0 DESCRIPTION "An example of how to use canyon.")

message("Building with CMake version: ${CMAKE_VERSION}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(_CRT_SECURE_NO_WARNINGS IMGUI_DEFINE_MATH_OPERATORS)

add_subdirectory(.. canyon)

include_directories(external/imgui)
include_directories(external/murmurhash.c)

set(SOURCES ${SOURCES}
	src/main.cpp
	src/example_app.cpp
	src/example_layer.cpp
	src/example_main.cpp
	src/ui_button.cpp
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

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE src)
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
target_link_libraries(${PROJECT_NAME} canyon)
target_link_libraries(${PROJECT_NAME} moth_ui)

if (MSVC)
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ignore:4099")
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/W3 /WX")
else()
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-Wall -Werror")
endif()

