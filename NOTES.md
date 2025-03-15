## 15/03/25

- Create moth factories in the applications.
- Platform vs window? Platform seems to not be used currently?
- Create a platform, create application (api agnostic), platform creates windows.
- Context probably shouldnt be loading assets. another class that uses context as a dep?
- Should sdl windows have their own renderer? yes. renderers are linked to windows

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

