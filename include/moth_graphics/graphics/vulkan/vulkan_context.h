#pragma once

#include <vulkan/vulkan_core.h>

// Forward-declare FT_Library to avoid requiring FreeType headers in consumers.
// FT_Library is typedef struct FT_LibraryRec_ *FT_Library in freetype/freetype.h.
struct FT_LibraryRec_;
typedef FT_LibraryRec_* FT_Library;

namespace moth_graphics::graphics::vulkan {
    class ManagedContext;

    /// @brief Holds a Vulkan instance and FreeType library.
    ///
    /// Constructed from existing handles — the caller retains ownership.
    /// Use ManagedContext (in src/) for moth-managed lifecycle.
    class Context {
        friend class ManagedContext;

    public:
        /// @brief Construct a context from existing Vulkan and FreeType handles.
        ///        The caller retains ownership of both.
        Context(VkInstance instance, FT_Library ftLibrary)
            : m_vkInstance(instance)
            , m_ftLibrary(ftLibrary) {
        }

        VkInstance GetInstance() const { return m_vkInstance; }
        FT_Library GetFTLibrary() const { return m_ftLibrary; }

    private:
        /// @brief For ManagedContext — sets members during Startup().
        void SetHandles(VkInstance instance, FT_Library ftLibrary) {
            m_vkInstance = instance;
            m_ftLibrary = ftLibrary;
        }

        VkInstance m_vkInstance = VK_NULL_HANDLE;
        FT_Library m_ftLibrary = nullptr;
    };
}
