from conan import ConanFile
from conan.tools.cmake import cmake_layout

class canyon(ConanFile):
    name = "canyon"
    version = "0.1"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps", "MSBuildToolchain", "MSBuildDeps"

    def requirements(self):
        self.requires("sdl/2.28.3")
        self.requires("sdl_image/2.0.5")
        self.requires("sdl_ttf/2.20.2")
        self.requires("glfw/3.3.8")
        self.requires("libpng/1.6.42", override=True)

    def build_requirements(self):
        self.tool_requires("cmake/3.27.9")

    def layout(self):
        cmake_layout(self)
