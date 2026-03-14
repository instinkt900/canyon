# TODO

Issues identified during code review. Work through each by verifying against current code before fixing.

---

## Vulkan Vertex Buffer: Arbitrary 1024-Vertex Limit

`BeginContext` and `RestartContext` both hard-code a 1024-vertex host-visible
buffer. `SubmitVertices` already flushes via `RestartContext()` when the
accumulator fills, but a single draw call larger than 1024 verts is currently
logged and dropped. The limit needs to be removed. Three options:

- **Option A — Larger fixed default**: Change 1024 → 65536 everywhere. Simple,
  removes the practical limit for all real UI workloads. Still technically
  bounded but would never be hit in practice.

- **Option B — Dynamic buffer resize**: When `vertCount > m_maxVertexCount`,
  unmap the old buffer, allocate a new one sized to the next power-of-two above
  `vertCount`, remap, and update `m_maxVertexCount`. Truly unbounded. The GPU
  fence wait needed for safe resize is already present in `RestartContext`.

- **Option C — Flush and submit in chunks**: When `vertCount > availableVertices`
  after a flush, split the draw into multiple passes of `m_maxVertexCount` verts
  each. Works because geometry is naturally chunkable (quads/triangles). Keeps
  the buffer fixed-size but hides the limit from callers entirely.

---

## Architectural / Known Issues (from NOTES.md)

- **Vulkan crashes on window close** unless all resources (images, fonts) linked to that context are destroyed first. Resources need to be per-window; assets created for one window will not work on another.
- **Vulkan exit cleanup crashes** — needs investigation.
- **SDL multi-window event handling** not yet properly implemented.
- **`SurfaceContext`** design could be improved — see NOTES.md for planned refactor.
