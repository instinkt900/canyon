from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake
from conan.tools.files import load

class canyon(ConanFile):
    name = "canyon"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"
    exports_sources = "CMakeLists.txt", "version.txt", "include/*", "src/*", "external/imgui/*", "external/murmurhash.c/*"
    package_type = "static-library"

    def set_version(self):
        self.version = load(self, "version.txt").strip()

    def requirements(self):
        self.requires("sdl/2.28.3")
        self.requires("sdl_image/2.0.5")
        self.requires("sdl_ttf/2.20.2")
        self.requires("glfw/3.3.8")
        self.requires("libpng/1.6.42", override=True)
        self.requires("vulkan-headers/1.3.243.0", transitive_headers=True)
        self.requires("vulkan-loader/1.3.243.0")
        self.requires("vulkan-memory-allocator/3.0.1")
        self.requires("freetype/2.13.2", transitive_headers=True)
        self.requires("spdlog/1.12.0")
        self.requires("harfbuzz/8.3.0")
        self.requires("moth_ui/0.1", transitive_headers=True)
        self.requires("fmt/10.2.1", override=True)

    def build_requirements(self):
        self.tool_requires("cmake/[>=3.27.0]")

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["canyon"]
        self.cpp_info.libdirs = ["lib"]
        self.cpp_info.includedirs = ["include"]
