## 19/04/25

- imgui implementation
- fix up example app. application tries to get window before run

## 18/04/25

- imgui implementation. make the context know whether imgui has a window or not.
- getting graphics from layer stack or something

## 05/04/25

- vulkan will crash on window close unless all resources related to that context are destroyed first
  - images, fonts, etc.
  - should be able to invalidate assets linked to that context?
  - this also means fonts/images etc. all need to be on a per window basis.
    - images and fonts created for one window will not work on another. can we fix this?
- SDL events will need to be handled properly with mutli windows
- Vulkan tiled rendering might need a review

## 04/04/25

- https://github.com/NVIDIA-RTX/NVRHI possible?

## 30/03/25

- Fixed font rendering on vulkan. was just blend modes. forced alpha in graphics.
- Need to fix vulkan exit cleanup. it crashes.
- Surface context can probably be something better
- sdl_utils.h shows errors because clangd is using SDL_FontCache.c as the compile target which is c. renaming it to hpp fixes it

## 29/03/25

- There should be an application context and a window context.
- Application context is things that can be used across all windows
- Window context is things like renderer for sdl.
- The factories are the problem. They are application wide but create assets that might be window context bound
Plan:
- Context - Application context. Holds application wide context for graphics apis
- SurfaceContext - Graphics context for a particular surface/window
- AssetLoader - Comes from Surface context and is used to get assets from disk
Each window will hold its own moth context that is set/unset on update/renders

maybe surface context is just graphics? not done yet
moth_ui updated to take the context into the node tree.
sdl working. vulkan working except fonts are weird.

## 27/03/25

- SDL context holds not much but vulkan context actually holds context
- Maybe move the asset loading stuff to something else
- set context? during window rendering?

## 16/03/25

- SDL font creation needs SDL_Renderer
- Vulkan font creation needs font shader which is on graphics
- ui_renderer needs graphics but graphics needs to be related to windows. ui_renderer is setup in platform
- need to change moth_ui interface. remove context and just have SetFontFactory, SetUIRender etc.
- SDL_Renderer should be on graphics
- will need to figure out font creation from that

## 15/03/25

- Create moth factories in the applications.
- Platform vs window? Platform seems to not be used currently?
- Create a platform, create application (api agnostic), platform creates windows.
- Context probably shouldnt be loading assets. another class that uses context as a dep?
- Should sdl windows have their own renderer? yes. renderers are linked to windows

- Graphics in vulkan needs to be associated to a window so platform cant store it.

## Older

- Separate out moth_ui stuff some more. Focus on the graphics interface without moth_ui then add a layer that adds ui support

- Refactor image handling.
    - Introduce image atlas type (texture?)
    - Images can be (texure?) or a sub section of the (texture?)
    - Will probably require refactoring the moth ui stuff and texture packing stuff.

- raylib implementation of moth_ui


- handling graphics images vs moth images
- current plan: create image factory (implementation agnostic) to get graphics images
- create moth ui image factory that is a wrapper around the above factory that provides moth_ui images
- moth_ui images are just wrappers around graphics images

- sort out context and graphics.
- context should have all the graphics api context stuff
- graphics should be focused on just drawing

- font factory is moth_ui
- want to be able to easily init moth_ui with just the canyon interface
- no user code should need to specify what graphics api to use except at initialisation
- thus will need api agnostic factories for moth_ui surface
- create an agnostic font factory
- create a mothui wrapper for the font factory
- the base font factory is created on platform creation?
- the wrapper takes the canyon font factory

