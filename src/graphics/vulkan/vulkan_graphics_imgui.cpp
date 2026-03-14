#include "common.h"
#include "canyon/platform/glfw/glfw_window.h"
#include "canyon/graphics/vulkan/vulkan_graphics.h"
#include "canyon/graphics/vulkan/vulkan_utils.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

namespace {
    void checkVkResult(VkResult err) {
        CHECK_VK_RESULT(err);
    }
}

namespace canyon::graphics::vulkan {
    void Graphics::InitImgui(canyon::platform::Window const& window) {
        if (m_imguiInitialized) {
            return;
        }

        auto const* glfwWindowPtr = dynamic_cast<canyon::platform::glfw::Window const*>(&window);
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

        ImGui_ImplGlfw_InitForVulkan(glfwWindow.GetGLFWWindow(), true);
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
        ImGui_ImplVulkan_Init(&initInfo);
        ImGui_ImplVulkan_CreateFontsTexture();

        m_imguiInitialized = true;
    }
}
