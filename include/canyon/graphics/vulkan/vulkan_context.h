#pragma once

#include "canyon/graphics/context.h"

#include <vulkan/vulkan_core.h>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace graphics::vulkan {
    class Context : public graphics::Context {
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
