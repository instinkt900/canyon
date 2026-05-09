#include "common.h"
#include "vulkan_managed_context.h"
#include "vulkan_utils.h"
#include <ft2build.h>
#include FT_FREETYPE_H

#include <spdlog/spdlog.h>

namespace {
    std::vector<char const*> const validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    bool const enableValidationLayers =
#ifdef NDEBUG
        false
#else
        true
#endif
        ;

    VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        VkDebugUtilsMessengerCallbackDataEXT const* pCallbackData,
        void* pUserData) {
        switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            spdlog::info("Validation Layer: {}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            spdlog::info("Validation Layer: {}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            spdlog::warn("Validation Layer: {}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            spdlog::error("Validation Layer: {}", pCallbackData->pMessage);
            break;
        default:
            break;
        }

        return VK_FALSE;
    }

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerCreateInfoEXT const* pCreateInfo, VkAllocationCallbacks const* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
        auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT")); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, VkAllocationCallbacks const* pAllocator) {
        auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT")); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr;
    }
}

#define _unused(x) ((void)(x))
#define FT_CHECK(r)         \
    {                       \
        FT_Error err = (r); \
        assert(!err);       \
        _unused(err);       \
    }                       \
    while (0)               \
        ;

namespace moth_graphics::graphics::vulkan {
    bool ManagedContext::Startup() {
        spdlog::info("Vulkan: initializing context");

        spdlog::info("Vulkan: initializing FreeType");
        FT_Library ftLibrary = nullptr;
        FT_CHECK(FT_Init_FreeType(&ftLibrary));

        // create instance
        VkInstance vkInstance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT vkDebugMessenger = VK_NULL_HANDLE;
        {
            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
            VkInstanceCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

            createInfo.enabledLayerCount = 0;
            if (enableValidationLayers) {
                bool success = true;

                uint32_t layerCount = 0;
                vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

                std::vector<VkLayerProperties> availableLayers(layerCount);
                vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

                for (char const* layerName : validationLayers) {
                    bool layerFound = false;

                    for (auto const& layerProperties : availableLayers) {
                        if (strcmp(layerName, layerProperties.layerName) == 0) {
                            layerFound = true;
                            break;
                        }
                    }

                    if (!layerFound) {
                        spdlog::error("Could not find validation layer {}.", layerName);
                        success = false;
                        break;
                    }
                }

                if (success) {
                    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
                    createInfo.ppEnabledLayerNames = validationLayers.data();
                }

                populateDebugMessengerCreateInfo(debugCreateInfo);
                createInfo.pNext = &debugCreateInfo;
            }

            uint32_t glfwExtensionCount = 0;
            char const** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
            if (glfwExtensions == nullptr) {
                spdlog::error("Vulkan: glfwGetRequiredInstanceExtensions returned nullptr");
                glfwExtensionCount = 0;
            }
            std::vector<char const*> extensions;
            if (glfwExtensions != nullptr) {
                extensions.assign(glfwExtensions, glfwExtensions + glfwExtensionCount);
            }

            if (enableValidationLayers) {
                extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            }

            createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
            createInfo.ppEnabledExtensionNames = extensions.data();
            CHECK_VK_RESULT(vkCreateInstance(&createInfo, nullptr, &vkInstance));
            spdlog::info("Vulkan: instance created");
        }

        if (enableValidationLayers) {
            spdlog::info("Vulkan: enabling validation layers");
            VkDebugUtilsMessengerCreateInfoEXT createInfo{};
            populateDebugMessengerCreateInfo(createInfo);
            CHECK_VK_RESULT(CreateDebugUtilsMessengerEXT(vkInstance, &createInfo, nullptr, &vkDebugMessenger));
        }
        spdlog::info("Vulkan: context ready");

        m_context.instance = vkInstance;
        m_context.ftLibrary = ftLibrary;
        m_debugMessenger = vkDebugMessenger;
        return true;
    }

    void ManagedContext::Shutdown() {
        spdlog::info("Vulkan: destroying context");
        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(m_context.instance, m_debugMessenger, nullptr);
        }
        vkDestroyInstance(m_context.instance, nullptr);
        FT_Done_FreeType(m_context.ftLibrary);
    }
}
