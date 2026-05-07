#include "common.h"
#include "graphics/vulkan/vulkan_graphics.h"
#include "graphics/vulkan/vulkan_utils.h"
#include "moth_graphics/platform/glfw/glfw_platform.h"
#include "moth_graphics/platform/glfw/glfw_window.h"
#include "moth_graphics/platform/imgui_context.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

namespace {

    void checkVkResult(VkResult err) {
        CHECK_VK_RESULT(err);
    }

}

namespace moth_graphics::platform::glfw {

    Platform::Platform() = default;

    Platform::~Platform() noexcept {
        ShutdownImpl();
    }

    namespace {
        class VulkanImGuiContext : public moth_graphics::platform::ImGuiContext {
        public:
            bool Init(moth_graphics::platform::Window& window, moth_graphics::graphics::IGraphics& graphics, bool enableViewports) override {
                auto* glfwWindowPtr = dynamic_cast<moth_graphics::platform::glfw::Window*>(&window);
                if (glfwWindowPtr == nullptr) {
                    spdlog::error("Vulkan: InitImgui called with non-GLFW window");
                    return false;
                }

                IMGUI_CHECKVERSION();
                ImGui::CreateContext();
                ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
                if (enableViewports) {
                    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
                }

                ImGui::StyleColorsDark();
                if (enableViewports) {
                    ImGuiStyle& style = ImGui::GetStyle();
                    style.WindowRounding = 0;
                    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
                }

                auto const& glfwWindow = *glfwWindowPtr;

                if (!ImGui_ImplGlfw_InitForVulkan(glfwWindow.GetGLFWWindow(), true)) {
                    spdlog::error("Vulkan: ImGui_ImplGlfw_InitForVulkan failed");
                    ImGui::DestroyContext();
                    return false;
                }

                auto* vkGraphicsPtr = dynamic_cast<moth_graphics::graphics::vulkan::Graphics*>(&graphics);
                if (vkGraphicsPtr == nullptr) {
                    spdlog::error("Vulkan: InitImgui called with non-Vulkan graphics");
                    ImGui_ImplGlfw_Shutdown();
                    ImGui::DestroyContext();
                    return false;
                }
                auto& vkGraphics = *vkGraphicsPtr;
                auto& surfaceContext = vkGraphics.GetSurfaceContext();

                ImGui_ImplVulkan_InitInfo initInfo{};
                initInfo.Instance = surfaceContext.GetContext().GetInstance();
                initInfo.PhysicalDevice = surfaceContext.GetVkPhysicalDevice();
                initInfo.Device = surfaceContext.GetVkDevice();
                initInfo.QueueFamily = surfaceContext.GetVkQueueFamily();
                initInfo.Queue = surfaceContext.GetVkQueue();
                initInfo.DescriptorPool = surfaceContext.GetVkDescriptorPool();
                initInfo.RenderPass = vkGraphics.GetRenderPass().GetRenderPass();
                initInfo.Subpass = 0;
                initInfo.MinImageCount = vkGraphics.GetSwapchain().GetImageCount();
                initInfo.ImageCount = vkGraphics.GetSwapchain().GetImageCount();
                initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
                initInfo.Allocator = nullptr;
                initInfo.CheckVkResultFn = checkVkResult;
                if (!ImGui_ImplVulkan_Init(&initInfo)) {
                    spdlog::error("Vulkan: ImGui_ImplVulkan_Init failed");
                    ImGui_ImplGlfw_Shutdown();
                    ImGui::DestroyContext();
                    return false;
                }

                ImGui_ImplVulkan_CreateFontsTexture();

                m_initialized = true;
                return true;
            }

            void NewFrame() override {
                if (m_initialized) {
                    ImGui_ImplVulkan_NewFrame();
                    ImGui_ImplGlfw_NewFrame();
                    ImGui::NewFrame();
                }
            }

            void Render(moth_graphics::graphics::IGraphics& graphics) override {
                if (!m_initialized) {
                    return;
                }
                ImGui::Render();
                if ((ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0) {
                    ImGui::UpdatePlatformWindows();
                    ImGui::RenderPlatformWindowsDefault();
                }
                if (ImDrawData* drawData = ImGui::GetDrawData()) {
                    auto* vkGraphicsPtr = dynamic_cast<moth_graphics::graphics::vulkan::Graphics*>(&graphics);
                    if (vkGraphicsPtr == nullptr) {
                        spdlog::warn("Vulkan: ImGui Render called with non-Vulkan graphics; skipping");
                        return;
                    }
                    // Flush any moth pending batch before ImGui binds its own
                    // pipeline; otherwise End()'s final flush would draw moth
                    // vertices using ImGui's pipeline.
                    vkGraphicsPtr->Flush();
                    ImGui_ImplVulkan_RenderDrawData(drawData, vkGraphicsPtr->GetCurrentCommandBuffer()->GetVkCommandBuffer());
                }
            }

            void DiscardFrame() override {
                if (m_initialized) {
                    ImGui::EndFrame();
                }
            }

            void Shutdown() override {
                if (m_initialized) {
                    ImGui_ImplVulkan_Shutdown();
                    ImGui_ImplGlfw_Shutdown();
                    ImGui::DestroyContext();
                    m_initialized = false;
                }
            }

        private:
            bool m_initialized = false;
        };
    }

    bool Platform::Startup() {
        if (glfwInit() == 0) {
            spdlog::error("GLFW: initialization failed");
            return false;
        }
        spdlog::info("GLFW: initialized");
        m_context = std::make_unique<graphics::vulkan::Context>();
        if (!m_context->Startup()) {
            spdlog::error("GLFW: graphics context startup failed");
            m_context.reset();
            glfwTerminate();
            return false;
        }
        m_initialized = true;
        return true;
    }

    void Platform::Shutdown() {
        ShutdownImpl();
    }

    void Platform::ShutdownImpl() {
        if (!m_initialized) {
            return;
        }
        spdlog::info("GLFW: shutting down");
        if (m_context) {
            m_context->Shutdown();
            m_context.reset();
        }
        glfwTerminate();
        m_initialized = false;
    }

    graphics::Context& Platform::GetGraphicsContext() {
        return *m_context;
    }

    std::unique_ptr<moth_graphics::platform::Window> Platform::CreateWindow(std::string_view title, int width, int height) {
        return std::make_unique<platform::glfw::Window>(*m_context, title, width, height);
    }

    std::unique_ptr<moth_graphics::platform::ImGuiContext> Platform::CreateImGuiContext() {
        return std::make_unique<VulkanImGuiContext>();
    }
}


