# Changelog

All notable changes to this project will be documented in this file.
Entries are generated automatically from git history using [git-cliff](https://github.com/orhun/git-cliff).

## [1.0.0-rc.1] - 2026-04-15
### Features
- Replace uniform-grid sheet model with per-frame rects and explicit clip steps
- Expose per-frame pivot on Sprite and add DrawSpriteAtPivot
- Add test suite for moth_graphics

### Bug Fixes
- Remove MothImage::ImGui() override following moth_ui IImage change
- Harden spritesheet loading and fix stale log/doc issues
- Address test suite review findings
- Name ImGui stub parameters and fix empty-sheet test image
- Removing unicode character from format
- Fix Windows test build — SDL_MAIN_HANDLED and /utf-8
- Provide SDL_main stub for Windows test build
- Force /SUBSYSTEM:CONSOLE in test binary to break SDL_main/WinMain cycle
- Clear SDL2::SDL2main INTERFACE_LINK_OPTIONS to prevent /SUBSYSTEM:WINDOWS
- Use /ENTRY:mainCRTStartup to force console entry point on Windows
- Use SDL main renaming instead of entry-point hacks on Windows
- Loosen SetRunning ticker test to >= 3 fixed ticks
- Allow pre-release suffix in cliff.toml tag_pattern

### Refactoring
- Rename DrawSpriteAtPivot to DrawSprite and drop default pivot

### Documentation
- Add known limitations to README, remove TODO

### Miscellaneous
- Resetting moth_ui dep version
- Updating moth_ui dep to accept rc versions
- Fixing moth_ui version dep. ranges dont work
- Remove shared library scaffolding — static-only build
- Updating version string parsing

### Changes
- Bump version to 1.0.0-rc.1

## [0.10.1] - 2026-04-04
### Features
- Register MothFlipbookFactory with moth_ui Context in Window::PostCreate

### Bug Fixes
- Route SDL input events to focused window in multi-window setups
- Use explicit nullptr check to suppress implicit-bool-conversion warning

### Changes
- Bump version from 0.10.0 to 0.10.1

## [0.10.0] - 2026-04-03
### Features
- Add SpriteSheet/SpriteSheetFactory and MothFlipbook/MothFlipbookFactory
- Add Sprite class with SpriteSheet, DrawSprite, and manual frame control
- Expose SpriteSheetFactory on AssetContext; add Sprite width/height accessors
- Add IFont::Measure and update DrawText to accept string_view

### Bug Fixes
- Update MothRenderer and MothImage to match IRenderer const change
- Remove spurious override on LoadTexturePack in MothImageFactory
- Flush SpriteSheetFactory cache and fallback image before VMA teardown
- Null-guard sprites, fix spriteRightPos width, batch Update, validate spritesheet JSON, fix GetCurrentFrameRect return type
- Clear m_playing on empty SetClip; add type and bounds checks in SpriteSheetFactory
- Use error_code filesystem calls and validate MaxFrames against sheet capacity
- Open spritesheet file and log errors using absPath consistently

### Refactoring
- Remove FlushCache override from MothImageFactory

### Miscellaneous
- Update example to use two sprites with Measure-based positioning
- Bumping moth_ui version dep
- Bounding moth_ui deps

### Changes
- Bump version from 0.9.0 to 0.10.0

## [0.9.0] - 2026-03-30
### Features
- Add spdlog error logging to all LoadTexturePack failure points
- Add pivot-based DrawImage overload and fix pivot offset sign

### Bug Fixes
- Parse moth_packer x/y/w/h rect format in LoadTexturePack
- Delete copy/move on AssetContext, fix LoadTexturePack return, update Window doc
- Flush factory caches before vmaDestroyAllocator in SurfaceContext destructor
- Correct MakeRect args in pivot-based DrawImage (width/height not x2/y2)

### Refactoring
- Move ImageFactory and FontFactory ownership into AssetContext

### Miscellaneous
- Standardize to US English spelling throughout

### Changes
- Update src/graphics/image_factory.cpp
- Bump version from 0.8.0 to 0.9.0

## [0.8.0] - 2026-03-25
### Features
- Add ImageFactory fallback image support and TextureFromPixels API
- Move transform stack into IGraphics; remove rotation param from DrawImage

### Bug Fixes
- Fetch tags after creation so git-cliff --current finds the tag
- Force-refresh tags on fetch to avoid stale refs on retry
- Use ../.conan/profile in example subdirectory
- Validate pixels pointer and destroy texture on SDL_LockTexture failure in TextureFromPixels
- Use SDL_PIXELFORMAT_RGBA32 instead of hardcoded ABGR8888 in TextureFromPixels
- Rotate text glyphs correctly in SDL and Vulkan backends
- PushTransform replaces rather than composes the active transform
- Restore external linkage for shader bytecode symbols
- Forward backend options to CMake and add validation in example recipe

### Refactoring
- Use kDegToRad/kRadToDeg from moth_ui namespace; re-export in moth_graphics
- Build example via add_subdirectory instead of conan package

### Performance
- Reuse scratch texture for rotated text; const shader bytecode

### Miscellaneous
- Bumping moth_ui dep version

### Changes
- Fix capitalization in project title
- Update src/graphics/image_factory.cpp
- Bump version from 0.7.0 to 0.8.0

## [0.7.0] - 2026-03-22
### Bug Fixes
- Fix build warnings and example dependency

### Refactoring
- Rename project from canyon to moth_graphics
- Rename canyon namespace and headers to moth_graphics

### Documentation
- Add full ecosystem table to Related Projects

### Miscellaneous
- Removed unused conan profiles

## [0.6.0] - 2026-03-21
### Features
- Updated loading of texture packs

### Miscellaneous
- Sorting out some linter issues
- Bump version

## [0.5.1] - 2026-03-19
### Documentation
- Add build, upload, and licence badges to README
- Add AI disclosure and table of contents to README
- Restructure README to match moth_ui layout

### Miscellaneous
- Bumping moth_ui dependency

### Changes
- Correct repository name from 'canyon' to 'Canyon'

## [0.5.0] - 2026-03-19
### Features
- Add canyon.h catch-all include and canyon_fwd.h forward declarations

### Bug Fixes
- Chunk oversized vertex submissions instead of dropping them
- Correct copy vs reference bugs in FontFactory and ImageFactory
- Review fixes — null guards, doc comments, topology-aware chunking
- Return false from LoadTexturePack when pack files do not exist
- Harden ImageFactory JSON validation and normalise cache keys
- Normalise cache keys with lexically_normal to prevent atlas lookup misses

### Refactoring
- Separate AssetContext from SurfaceContext

### Miscellaneous
- Updating .clangd to ignore unused includes in catchall headers
- Removing NOTES.md
- Updating TODO.md
- Bumped version to 0.5.0
- Removed unused variable

### Changes
- Enable precompiled headers using src/common.h
- Simplify to a minimal how-to application

## [0.4.0] - 2026-03-15
### Features
- Add moth_ui_format.h with fmt formatters for all moth_ui types

### Bug Fixes
- MothImage::ImGui was a no-op, breaking image preview in properties panel
- MothImage::ImGui was a no-op, breaking image preview in properties panel

### Refactoring
- Include type name in enum formatter output

### Miscellaneous
- Add git-cliff changelog automation and expand TODO

### Changes
- Updates from moth_ui release. Bringing into better state.
- Revert "fix: MothImage::ImGui was a no-op, breaking image preview in properties panel"

## [0.3.0] - 2026-03-04
### Bug Fixes
- Vulkan synchronization and shutdown correctness
- Remove FreeType headers from public canyon headers
- Wire LayerStack event listener in Application::Init()
- Use libfreetype-dev apt package name for Ubuntu 24.04+

### Changes
- Aliasing moth ui types instead of duplicating and converting.
- Decoupling the available semaphore from the swapchain image.
- Frame-slot

## [0.2.0] - 2025-05-10
### Features
- Updated moth renderer to have logical size support.
- Adding back imgui image rendering

### Bug Fixes
- Fix: Fixed font loading for moth font factory.
- Fix: Fixed the incorrect rendering of images in vulkan.
- Fixed sdl font rendering
- Fixed font rendering on vulkan
- SDL rendering with multiple windows
- Vulkan tiled rendering.
- Fixing windows builds.
- Updating some header exposures
- Fixing upload action

### Refactoring
- Refactor(font_factory): Adding new moth font factory.
- Refactor(font_factory): Removing api specific font factories.
- Refactor(font_factory): Internal implementation of font factory.
- Refactor(image_factory): Moth image factory now just a wrapper.
- Refactor(factories): Removed old implementation specific factories.
- Refactor(moth_ui/wrapper): Unified ui_renderer
- Refactor(graphics/utilities): Cleaning up some utility conversions.
- Refactor(graphics/vulkan): Misc vulkan cleanups.
- Refactor(platform): Cleaning up platform, application, window layout.
- Refactor(main): SDL portion working.
- Refactor(font/vulkan): Font no longer needs graphics.
- Refactor(graphics): Moving asset loading out of graphics.
- Renamed ui_renerer to MothRenderer
- Small adjustments to how events are defined
- Added graphics contexts to windows.
- The moth context is now passed into the ui node tree.
- Just some formatting stuff and minor cleanups.
- Moved example code into its own location
- Moved the button widget
- Added an example main function that draws two windows
- Removed canyon layers in favor of moth_ui layers.

### Miscellaneous
- Updated build file list
- Updated notes
- Chore(cleanup): Deleting unneeded files.
- Misc things before refactor.
- Making compile define private

### Changes
- Added SDL and GLFW abstractions to creating a window. Will be changing how this works soon
- Adding in sdl graphics wrappings
- Just switching for testing
- Ignoring warnings in fontcache
- Fixing up some ordering. Not happy with the current setup but it works for now
- Simplifying things
- Added vulkan work
- Some stuff
- Working glfw and sdl implementations
- Moving away from inheritence chains
- Adding event_emitter
- Adding precompiled header. Cleaned up main
- Fixing vulkan resize, Adding layers to test applications
- Well it's building
- Building and running (SDL)
- Removing unused font factory header
- Fixing up vulkan
- Moving graphics and layer stacks into window which seems more appropriate
- Added loading methods for image and font
- Cleaning up a little bit
- More cleanup. glfw events was duplicated in vulkan events. Moving some source around
- WIP commit so i can swap OSes
- Working with SDL and Vulkan.
- Making platform independent developing easier
- So much stuff... Separating out graphics stuff and moth_ui stuff still
- Fixed event handling for multiple sdl windows.
- Turning canyon into more of a consumable library.
- Imgui implementation.
- ITargets are now IImages.
- Fixing ITarget usage.
- Cleaning up building and packaging.
- Adding actions to build and upload artifacts.
- Updates for moth_ui api change.


