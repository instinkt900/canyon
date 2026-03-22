#pragma once

#include "moth_graphics/graphics/asset_context.h"

namespace moth_graphics::graphics {
    /// @brief Per-window GPU resource context.
    ///
    /// Owns the GPU device resources associated with a single window surface
    /// (command pools, descriptor pools, allocators, etc.). Use GetAssetContext()
    /// to load GPU-backed assets (textures, images, fonts).
    class SurfaceContext {
    public:
        virtual ~SurfaceContext() = default;

        /// @brief Returns the asset loading interface for this surface.
        virtual AssetContext& GetAssetContext() = 0;
    };
}
