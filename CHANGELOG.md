# Changelog

All notable changes to this project will be documented in this file.
Entries are generated automatically from git history using [git-cliff](https://github.com/orhun/git-cliff).

## [1.0.0] - 2026-05-11
### Features
- Add font metrics to IFont, switch SpriteSheet getters to std::optional
- Standalone graphics without Window/Application dependency

### Bug Fixes
- Update LayerStack constructor call for IntVec2 API
- Guard PopClip against empty stack and rename textureFilter member
- Make Texture lazy-create methods const
- Guard GLFW DestroyWindow against null window handle
- Remap DrawImGui UVs through Image source rect
- GetTextureRect returns fallback source rect on cache miss
- Strengthen SpriteSheet GetImage round-trip test with mock texture
- Only swizzle BGRA→RGBA in Vulkan SaveToPNG when source is BGRA
- DrawSprite takes Sprite const& for const-correctness
- Sync backend transform with renderer stack on construction
- Use nullptr for FC_GetAscent/GetDescent to get font-level metrics
- Add NOLINT suppression for FC_GetAscent/GetDescent vararg calls
- Match IFont override return types, deduplicate sprite frame lookups
- Guard ImGui Render on Draw success, check ImGuiContext init return
- Update test to match Window::Draw returning bool
- Clean up platform resources when Context::Startup fails, guard Shutdown
- Don't compose transforms in MothRenderer::PushTransform
- Assert valid surface context in glfw Window::GetSurfaceContext
- Initialise Platform m_context in-class to satisfy member-init lint
- Replace throwing dynamic_cast<T&> with logged null-check pattern
- Correct ImGui frame lifecycle and shutdown ordering
- Add null guards and eliminate per-frame dynamic_cast in ImGui context
- Make Update pure virtual and move owned pointers to private
- Assert SetImGuiViewportsEnabled is called before Init()
- Remove dead FlushCache/LoadTexturePack from MothImageFactory
- Rename GetFont name->path and don't cache failed loads
- Null-guard MothFont construction and validate pivot JSON types
- Strict JSON validation for clips field and pivot types
- Tighten Vulkan error handling and ImGui init ordering
- Forward-declare SurfaceContext in correct namespace
- Remove redundant ImGui_ImplVulkan_CreateFontsTexture call
- Add null guards and error-result checks across Vulkan/GLFW backends
- Add null guards and explicit member init in SDL/Vulkan surface constructors
- Move ImGuiContext ownership into Window to fix exit-time SIGSEGV
- Throw from vulkan SurfaceContext ctor instead of abort()
- Address review — Texture destructor order and SetAddressMode waitIdle
- Destroy VkPipeline before releasing the shared Shader

### Refactoring
- Make vulkan Image members private, expose accessors
- Convert IImage to Image value type, ImageFactory to TextureFactory
- Move DrawSprite overloads off IGraphics to free functions
- Make Sprite a regular value type with public constructor
- Replace IGraphics PushTransform/PopTransform with SetTransform
- Extract ImGui from IGraphics into platform-level ImGuiContext
- Move Vulkan/SDL backend impl headers from public include/ to private src/
- Move remaining SDL backend impl headers to private src/
- Make all Vulkan and SDL backend headers private
- Restore unique_ptr<Context> in Platform classes
- Forward-declare Texture in vulkan_shader.h
- Rename iimage.h to image.h to match its type
- Extract sprite sheet JSON parsing into named helpers
- Merge ImGuiContext::Init into CreateImGuiContext
- Remove GetSurfaceContext and Drain from IGraphics
- Make IGraphics::Begin void with self-healing null frames
- Make vulkan::Context a POD struct, remove graphics::Context base
- Remove sdl::Context entirely, was an empty zombie type
- Introduce UniqueHandle<T> RAII wrapper for internal Vulkan handles

### Documentation
- Fix README example to compile against current API
- Complete the moth_graphics.h umbrella header
- Note O(n²) behaviour in EventEmitter::EmitEvent
- Fix stale docstrings in factory and surface context headers
- Correct ImGui integration description in README

### Testing
- Remove MothImageFactory FlushCache/LoadTexturePack pins

### Miscellaneous
- Removing stale review doc
- Rename canyon_events.h to moth_graphics_events.h and minor housekeeping
- Bump moth_ui dep
- Bump moth_ui dep for tests
- Give Context Startup/Shutdown, move handlers private, expose LayerStack
- Remove redundant explicitly-defaulted Sprite copy/move
- Remove stale TODO from Vulkan render pass format
- Fix naming inconsistencies and dead code (review nits)
- Remove accidentally committed REVIEW.md

### Changes
- Use local include in sdl_surface_context.h
- Update version from 1.0.0-rc.1 to 1.0.0

## [1.0.0-rc.1] - 2026-04-28
### Features
- Replace uniform-grid sheet model with per-frame rects and explicit clip steps
- Expose per-frame pivot on Sprite and add DrawSpriteAtPivot
- Add test suite for moth_graphics
- Make ImGui multi-viewport opt-in via enableViewports param
- Implement PushTextureFilter/PopTextureFilter in MothRenderer

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
- Default texture filter to linear in SDL and Vulkan constructors
- Handle Vulkan swapchain out-of-date on window resize
- Update api_surface_graphics test to match current IGraphics signatures
- Propagate nullptr from MothImageFactory when inner image load fails
- Intersecting clips now no longer get negative bounds.
- Correct per-node texture filtering when two nodes share the same image
- Use DPI-aware font sizing on Vulkan/GLFW backend
- Re-enable screensaver after SDL init
- Set SDL_HINT_VIDEO_ALLOW_SCREENSAVER before SDL_Init to prevent screensaver suppression
- Skip draw calls with zero or negative destination rect dimensions
- Adding a bit of a fudge factor to vulkan font sizing
- Update API surface tests for string_view parameter sweep
- Guard PushLayer against pre-PostCreate use and deduplicate GLFW event dispatch
- Use pixel sizing for Vulkan fonts to match SDL_ttf behaviour

### Refactoring
- Rename DrawSpriteAtPivot to DrawSprite and drop default pivot
- Use string_view for title and text parameters in public API
- Rename CanyonEventType to MothGraphicsEventType
- Route all events through LayerStack before external listeners
- EventListener -> IEventListener, remove m_ prefix from struct/public members

### Performance
- Skip sampler teardown in SetFilter/SetAddressMode when unchanged
- Cache SDL scale mode in Texture to avoid redundant flush+set calls

### Documentation
- Add known limitations to README, remove TODO
- Clarify font size parameter is pixels, not points

### Testing
- Pin Window EventListener base and new public methods

### Miscellaneous
- Resetting moth_ui dep version
- Updating moth_ui dep to accept rc versions
- Fixing moth_ui version dep. ranges dont work
- Remove shared library scaffolding — static-only build
- Updating version string parsing
- Removed example. moth_example will take its place
- Exposing version info in the api
- Adding license file

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


