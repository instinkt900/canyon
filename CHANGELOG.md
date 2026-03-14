# Changelog

All notable changes to this project will be documented in this file.
Entries are generated automatically from git history using [git-cliff](https://github.com/orhun/git-cliff).

## [Unreleased]
### Bug Fixes
- Vulkan synchronization and shutdown correctness
- Remove FreeType headers from public canyon headers
- Wire LayerStack event listener in Application::Init()
- Use libfreetype-dev apt package name for Ubuntu 24.04+

### Changes
- Decoupling the available semaphore from the swapchain image.
- Frame-slot
- Updates from moth_ui release. Bringing into better state.

## [0.3.0] - 2025-08-14
### Changes
- Aliasing moth ui types instead of duplicating and converting.

## [0.2.0] - 2025-05-10
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

### Features
- Updated moth renderer to have logical size support.
- Adding back imgui image rendering

### Miscellaneous
- Updated build file list
- Updated notes
- Chore(cleanup): Deleting unneeded files.
- Misc things before refactor.
- Making compile define private

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


