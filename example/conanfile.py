from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.cmake import CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.system.package_manager import Apt


class MothGraphicsExample(ConanFile):
    name = "moth_graphics-example"
    description = "An example for how to use moth_graphics."
    version = "1.0"
    settings = "os", "compiler", "build_type", "arch"
    package_type = "application"

    options = {
        "disable_vulkan": [True, False],
        "disable_sdl": [True, False],
    }
    default_options = {
        "disable_vulkan": False,
        "disable_sdl": False,
    }

    def validate(self):
        if self.options.disable_vulkan and self.options.disable_sdl:
            raise ConanInvalidConfiguration("disable_vulkan and disable_sdl cannot both be True")

    def requirements(self):
        # moth_graphics is built from source via add_subdirectory; list its
        # external Conan dependencies here so they are resolved by this profile.
        self.requires("spdlog/[~1.14]", transitive_headers=True)
        self.requires("moth_ui/[>=1.6.0 <2.0.0]", transitive_headers=True)
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

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.cache_variables["MOTH_GRAPHICS_DISABLE_VULKAN"] = bool(self.options.disable_vulkan)
        tc.cache_variables["MOTH_GRAPHICS_DISABLE_SDL"] = bool(self.options.disable_sdl)
        tc.generate()

    def layout(self):
        cmake_layout(self)
