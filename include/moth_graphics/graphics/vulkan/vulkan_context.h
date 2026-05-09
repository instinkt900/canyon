#pragma once

#include <vulkan/vulkan_core.h>

// Forward-declare FT_Library to avoid requiring FreeType headers in consumers.
// FT_Library is typedef struct FT_LibraryRec_ *FT_Library in freetype/freetype.h.
struct FT_LibraryRec_;
typedef FT_LibraryRec_* FT_Library;

namespace moth_graphics::graphics::vulkan {
    /// @brief Holds a Vulkan instance and FreeType library.
    ///
    /// Aggregate — construct with {instance, ftLibrary}. The caller retains
    /// ownership of both handles. Use ManagedContext (in src/) for
    /// moth-managed lifecycle.
    struct Context {
        VkInstance instance = VK_NULL_HANDLE;
        FT_Library ftLibrary = nullptr;
    };
}
