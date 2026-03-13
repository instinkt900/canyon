# TODO

Issues identified during code review. Work through each by verifying against current code before fixing.

---

## Architectural / Known Issues (from NOTES.md)

- **Vulkan crashes on window close** unless all resources (images, fonts) linked to that context are destroyed first. Resources need to be per-window; assets created for one window will not work on another.
- **Vulkan exit cleanup crashes** — needs investigation.
- **SDL multi-window event handling** not yet properly implemented.
- **`SurfaceContext`** design could be improved — see NOTES.md for planned refactor.
