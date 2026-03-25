from conan import ConanFile
from conan.tools.cmake import cmake_layout
from conan.tools.system.package_manager import Apt


class MothGraphicsExample(ConanFile):
    name = "moth_graphics-example"
    description = "An example for how to use moth_graphics."
    version = "1.0"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"
    package_type = "application"

    options = {
        "disable_vulkan": [True, False],
        "disable_sdl": [True, False],
    }
    default_options = {
        "disable_vulkan": False,
        "disable_sdl": False,
    }

    def requirements(self):
        # moth_graphics is built from source via add_subdirectory; list its
        # external Conan dependencies here so they are resolved by this profile.
        self.requires("spdlog/[~1.14]", transitive_headers=True)
        self.requires("moth_ui/1.5.0", transitive_headers=True)
        if not self.options.disable_sdl and self.settings.os == "Windows":
            self.requires("sdl/[~2.28]", override=True, transitive_headers=True)
            self.requires("sdl_image/[~2.0]")
            self.requires("sdl_ttf/[~2.20]")
        if not self.options.disable_vulkan:
            if self.settings.os == "Windows":
                self.requires("glfw/3.3.8", transitive_headers=True)
                self.requires("freetype/[~2.13]", transitive_headers=True)
                self.requires("harfbuzz/[~8.3]", transitive_headers=True)
            self.requires("vulkan-headers/1.3.243.0", transitive_headers=True)
            self.requires("vulkan-loader/1.3.243.0")
            self.requires("vulkan-memory-allocator/3.0.1", transitive_headers=True)

    def system_requirements(self):
        if self.settings.os == "Linux":
            import shutil
            packages = []
            if not self.options.disable_sdl:
                packages += ["libsdl2-dev", "libsdl2-image-dev", "libsdl2-ttf-dev"]
            if not self.options.disable_vulkan:
                packages += ["libglfw3-dev", "libfreetype-dev", "libharfbuzz-dev"]
            if packages:
                packages.append("pkg-config")
                apt = Apt(self)
                apt.install(packages)

    def build_requirements(self):
        self.tool_requires("cmake/3.27.0")

    def layout(self):
        cmake_layout(self)
