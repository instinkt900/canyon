#pragma once

#include "canyon/graphics/context.h"

#include <vulkan/vulkan_core.h>

// Forward-declare FT_Library to avoid requiring FreeType headers in consumers.
// FT_Library is typedef struct FT_LibraryRec_ *FT_Library in freetype/freetype.h.
struct FT_LibraryRec_;
typedef FT_LibraryRec_* FT_Library;

namespace canyon::graphics::vulkan {
    class Context : public canyon::graphics::Context {
    public:
        Context();
        virtual ~Context();

        VkInstance const& GetInstance() const { return m_vkInstance; }
        FT_Library const& GetFTLibrary() const { return m_ftLibrary; }

    private:
        VkInstance m_vkInstance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT m_vkDebugMessenger = VK_NULL_HANDLE;
        FT_Library m_ftLibrary;
    };
}
