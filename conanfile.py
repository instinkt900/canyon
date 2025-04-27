from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.files import load

class canyon(ConanFile):
    name = "canyon"

    license = "MIT"
    url = "https://github.com/instinkt900/canyon"
    description = "A basic graphical application framework that uses moth_ui"

    settings = "os", "compiler", "build_type", "arch"
    options = { "shared": [True, False], "fPIC": [True, False] }
    default_options = { "shared": False, "fPIC": True }
    package_type = "library"

    exports_sources = "CMakeLists.txt", "version.txt", "include/*", "src/*", "external/imgui/*", "external/murmurhash.c/*"

    def set_version(self):
        self.version = load(self, "version.txt").strip()

    def requirements(self):
        self.requires("sdl/[~2.28]", override=True, transitive_headers=True)
        self.requires("sdl_image/[~2.0]")
        self.requires("sdl_ttf/[~2.20]")
        self.requires("glfw/3.3.8", transitive_headers=True)
        self.requires("vulkan-headers/1.3.243.0", transitive_headers=True)
        self.requires("vulkan-loader/1.3.243.0")
        self.requires("vulkan-memory-allocator/3.0.1", transitive_headers=True)
        self.requires("freetype/[~2.13]", transitive_headers=True)
        self.requires("spdlog/[~1.14]", transitive_headers=True)
        self.requires("harfbuzz/[~8.3]")
        self.requires("moth_ui/[<1.0.0]", transitive_headers=True)

    def build_requirements(self):
        self.tool_requires("cmake/[>=3.27.0]")

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

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
        self.cpp_info.includedirs = ["include", "external/imgui"]
        self.cpp_info.defines = ["IMGUI_DEFINE_MATH_OPERATORS"]

