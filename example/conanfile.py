from conan import ConanFile
from conan.tools.cmake import cmake_layout

class CanyonExample(ConanFile):
    name = "canyon-example"
    description = "An example for how to use canyon."
    version = "1.0"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"
    package_type = "application"

    def requirements(self):
        self.requires("canyon/0.2.0")

    def build_requirements(self):
        self.tool_requires("cmake/[>=3.27.0]")

    def layout(self):
        cmake_layout(self)

