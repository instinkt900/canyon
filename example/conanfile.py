from conan import ConanFile
from conan.tools.cmake import cmake_layout

class MothGraphicsExample(ConanFile):
    name = "moth_graphics-example"
    description = "An example for how to use moth_graphics."
    version = "1.0"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"
    package_type = "application"

    def requirements(self):
        self.requires("moth_graphics/0.6.0")

    def build_requirements(self):
        self.tool_requires("cmake/3.27.0")

    def layout(self):
        cmake_layout(self)

