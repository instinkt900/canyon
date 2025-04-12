from conan import ConanFile
from conan.tools.cmake import cmake_layout

class CanyonExample(ConanFile):
    name = "canyon-example"
    version = "1.0"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("canyon/0.1.0")
        self.requires("nlohmann_json/3.11.2")
        self.requires("vulkan-loader/1.3.243.0")

    def build_requirements(self):
        self.tool_requires("cmake/3.27.9")

    def layout(self):
        cmake_layout(self)

