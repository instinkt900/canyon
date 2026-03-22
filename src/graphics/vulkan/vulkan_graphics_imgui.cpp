#include "common.h"
#include "moth_graphics/platform/glfw/glfw_window.h"
#include "moth_graphics/graphics/vulkan/vulkan_graphics.h"
#include "moth_graphics/graphics/vulkan/vulkan_utils.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

namespace {
    void checkVkResult(VkResult err) {
        CHECK_VK_RESULT(err);
    }
}

namespace moth_graphics::graphics::vulkan {
    void Graphics::InitImgui(moth_graphics::platform::Window const& window) {
        if (m_imguiInitialized) {
            return;
        }

        auto const* glfwWindowPtr = dynamic_cast<moth_graphics::platform::glfw::Window const*>(&window);
        if (glfwWindowPtr == nullptr) {
            spdlog::error("Vulkan: InitImgui called with non-GLFW window");
            return;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();
        if ((ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0) {
            style.WindowRounding = 0;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        auto const& glfwWindow = *glfwWindowPtr;

        if (!ImGui_ImplGlfw_InitForVulkan(glfwWindow.GetGLFWWindow(), true)) {
            spdlog::error("Vulkan: ImGui_ImplGlfw_InitForVulkan failed");
            ImGui::DestroyContext();
            return;
        }

        ImGui_ImplVulkan_InitInfo initInfo{};
        initInfo.Instance = m_surfaceContext.GetContext().GetInstance();
        initInfo.PhysicalDevice = m_surfaceContext.GetVkPhysicalDevice();
        initInfo.Device = m_surfaceContext.GetVkDevice();
        initInfo.QueueFamily = m_surfaceContext.GetVkQueueFamily();
        initInfo.Queue = m_surfaceContext.GetVkQueue();
        initInfo.DescriptorPool = m_surfaceContext.GetVkDescriptorPool();
        initInfo.RenderPass = m_renderPass->GetRenderPass();
        initInfo.Subpass = 0;
        initInfo.MinImageCount = m_swapchain->GetImageCount();
        initInfo.ImageCount = m_swapchain->GetImageCount();
        initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        initInfo.Allocator = nullptr;
        initInfo.CheckVkResultFn = checkVkResult;
        if (!ImGui_ImplVulkan_Init(&initInfo)) {
            spdlog::error("Vulkan: ImGui_ImplVulkan_Init failed");
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
            return;
        }

        ImGui_ImplVulkan_CreateFontsTexture();

        m_imguiInitialized = true;
    }
}
