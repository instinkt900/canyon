from conan import ConanFile
from conan.tools.cmake import cmake_layout
from conan.tools.system.package_manager import Apt


class MothGraphicsTests(ConanFile):
    name = "moth_graphics_tests"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("catch2/3.13.0")
        # moth_graphics is built from source via add_subdirectory;
        # list its external Conan dependencies here.
        self.requires("moth_ui/1.0.0-rc.1", transitive_headers=True)
        self.requires("spdlog/[~1.14]", transitive_headers=True)
        # SDL2/SDL_image/SDL_ttf and GLFW/Freetype/HarfBuzz come from the system
        # package manager on Linux (GTK3/GDK-Pixbuf conflict). On Windows they
        # come from Conan.
        if self.settings.os == "Windows":
            self.requires("sdl/[~2.28]", override=True, transitive_headers=True)
            self.requires("sdl_image/[~2.0]")
            self.requires("sdl_ttf/[~2.20]")
            self.requires("glfw/3.3.8", transitive_headers=True)
            self.requires("freetype/[~2.13]", transitive_headers=True)
            self.requires("harfbuzz/[~8.3]", transitive_headers=True)
        # Vulkan packages have no GTK3 conflict — use Conan on both platforms.
        self.requires("vulkan-headers/1.3.243.0", transitive_headers=True)
        self.requires("vulkan-loader/1.3.243.0")
        self.requires("vulkan-memory-allocator/3.0.1", transitive_headers=True)

    def system_requirements(self):
        if self.settings.os == "Linux":
            packages = [
                "libsdl2-dev", "libsdl2-image-dev", "libsdl2-ttf-dev",
                "libglfw3-dev", "libfreetype-dev", "libharfbuzz-dev",
                "pkg-config",
            ]
            apt = Apt(self)
            apt.install(packages)

    def build_requirements(self):
        self.tool_requires("cmake/[>=3.27.0]")

    def layout(self):
        cmake_layout(self)
