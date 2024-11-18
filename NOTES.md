# TODO
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

