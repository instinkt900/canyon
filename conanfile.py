from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.files import load
from conan.tools.system.package_manager import Apt

class MothGraphics(ConanFile):
    name = "moth_graphics"

    license = "MIT"
    url = "https://github.com/instinkt900/moth_graphics"
    description = "A basic graphical application framework that uses moth_ui"

    settings = "os", "compiler", "build_type", "arch"
    package_type = "static-library"

    options = {
        "disable_vulkan": [True, False],
        "disable_sdl": [True, False],
    }
    default_options = {
        "disable_vulkan": False,
        "disable_sdl": False,
    }

    exports_sources = "CMakeLists.txt", "version.txt", "include/*", "src/*", "external/imgui/*", "external/murmurhash.c/*", "external/stb/*"

    def set_version(self):
        if not self.version:
            self.version = load(self, "version.txt").strip()

    def validate(self):
        if self.options.disable_vulkan and self.options.disable_sdl:
            raise ConanInvalidConfiguration("disable_vulkan and disable_sdl cannot both be True")

    def requirements(self):
        # SDL2, SDL_image, SDL_ttf, and GLFW bring in the system display/audio
        # stack (X11, Wayland, ALSA, libpng, libjpeg, etc.) which must match the
        # system versions already used by GTK3/GDK-Pixbuf (via NFD). Using
        # Conan-built copies causes runtime symbol conflicts. On Linux these must
        # come from the system package manager.
        if not self.options.disable_sdl and self.settings.os == "Windows":
            self.requires("sdl/[~2.28]", override=True, transitive_headers=True)
            self.requires("sdl_image/[~2.0]")
            self.requires("sdl_ttf/[~2.20]")
        if not self.options.disable_vulkan:
            if self.settings.os == "Windows":
                self.requires("glfw/3.3.8", transitive_headers=True)
                # vulkan_context.h includes <ft2build.h> and vulkan_font.h includes
                # <harfbuzz/hb.h> so consumers need both headers. On Linux these
                # come from the system package manager.
                self.requires("freetype/[~2.13]", transitive_headers=True)
                self.requires("harfbuzz/[~8.3]", transitive_headers=True)
            self.requires("vulkan-headers/1.3.243.0", transitive_headers=True)
            self.requires("vulkan-loader/1.3.243.0")
            self.requires("vulkan-memory-allocator/3.0.1", transitive_headers=True)
        self.requires("spdlog/[~1.14]", transitive_headers=True)
        self.requires("moth_ui/1.0.0", transitive_headers=True)

    def system_requirements(self):
        if self.settings.os == "Linux":
            import shutil
            packages = []
            if not self.options.disable_sdl:
                packages += ["libsdl2-dev", "libsdl2-image-dev", "libsdl2-ttf-dev"]
            if not self.options.disable_vulkan:
                packages += ["libglfw3-dev", "libfreetype-dev", "libharfbuzz-dev"]
            if not self.options.disable_sdl or not self.options.disable_vulkan:
                packages.append("pkg-config")
            if packages:
                if not shutil.which("apt-get"):
                    raise ConanInvalidConfiguration(
                        "apt-get is required to install system libraries on Linux. "
                        "Install the following manually via your system package manager: "
                        + ", ".join(packages)
                    )
                apt = Apt(self)
                apt.install(packages)

    def build_requirements(self):
        self.tool_requires("cmake/3.27.0")

    def layout(self):
        cmake_layout(self)
        # Added this here so that the package in editable mode still works with the extra includes.
        self.cpp.source.includedirs.append("external/imgui")

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.cache_variables["MOTH_GRAPHICS_DISABLE_VULKAN"] = bool(self.options.disable_vulkan)
        tc.cache_variables["MOTH_GRAPHICS_DISABLE_SDL"] = bool(self.options.disable_sdl)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["moth_graphics"]
        self.cpp_info.libdirs = ["lib"]
        self.cpp_info.includedirs = ["include", "external/imgui"]
        self.cpp_info.defines = ["IMGUI_DEFINE_MATH_OPERATORS"]
        self.cpp_info.defines.append("MOTH_GRAPHICS_DISABLE_VULKAN={}".format(1 if self.options.disable_vulkan else 0))
        self.cpp_info.defines.append("MOTH_GRAPHICS_DISABLE_SDL={}".format(1 if self.options.disable_sdl else 0))
        if self.settings.os == "Linux":
            # System SDL2/SDL_image/SDL_ttf/GLFW — propagate link flags and
            # include paths to all consumers. Paths are detected via
            # pkg-config rather than hard-coding a sysroot-relative location.
            import shutil
            import subprocess
            system_libs = []
            pkg_config_pkgs = []
            if not self.options.disable_sdl:
                system_libs += ["SDL2", "SDL2_image", "SDL2_ttf"]
                pkg_config_pkgs.append("sdl2")
            if not self.options.disable_vulkan:
                system_libs += ["glfw", "freetype", "harfbuzz"]
                pkg_config_pkgs += ["freetype2", "harfbuzz"]
            if system_libs:
                self.cpp_info.system_libs = system_libs
            pkg_config = shutil.which("pkg-config")
            if pkg_config_pkgs:
                if not pkg_config:
                    raise ConanInvalidConfiguration(
                        "pkg-config is required to locate system library headers "
                        f"for: {', '.join(pkg_config_pkgs)}. "
                        "Install it with: sudo apt install pkg-config"
                    )
                try:
                    flags = subprocess.check_output(
                        [pkg_config, "--cflags-only-I", *pkg_config_pkgs],
                        text=True
                    ).split()
                    for flag in flags:
                        if flag.startswith("-I"):
                            self.cpp_info.includedirs.append(flag[2:])
                except subprocess.SubprocessError as e:
                    raise ConanInvalidConfiguration(
                        f"pkg-config query failed for {', '.join(pkg_config_pkgs)}; "
                        "ensure the required system development headers are installed. "
                        f"Error: {e}"
                    ) from e

