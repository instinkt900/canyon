#include "common.h"
#include "graphics/sdl/sdl_context.h"
#include "moth_graphics/platform/imgui_context.h"
#include "moth_graphics/platform/sdl/sdl_window.h"
#include "moth_graphics/platform/sdl/sdl_platform.h"

#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"

namespace moth_graphics::platform::sdl {

    Platform::Platform() = default;

    Platform::~Platform() noexcept {
        ShutdownImpl();
    }

    namespace {
        class SDLImGuiContext : public moth_graphics::platform::ImGuiContext {
        public:
            bool Init(moth_graphics::platform::Window& window, moth_graphics::graphics::IGraphics& /*graphics*/, bool /*enableViewports*/) override {
                auto* sdlWindowPtr = dynamic_cast<moth_graphics::platform::sdl::Window*>(&window);
                if (sdlWindowPtr == nullptr) {
                    spdlog::error("SDL: InitImgui called with non-SDL window");
                    return false;
                }

                IMGUI_CHECKVERSION();
                ImGui::CreateContext();
                auto& io = ImGui::GetIO();
                io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
                ImGui::StyleColorsDark();

                auto& sdlWindow = *sdlWindowPtr;
                m_sdlWindow = sdlWindow.GetSDLWindow();
                ImGui_ImplSDL2_InitForSDLRenderer(sdlWindow.GetSDLWindow(), sdlWindow.GetSDLRenderer());
                ImGui_ImplSDLRenderer2_Init(sdlWindow.GetSDLRenderer());
                return true;
            }

            void NewFrame() override {
                if (m_sdlWindow != nullptr) {
                    ImGui_ImplSDLRenderer2_NewFrame();
                    ImGui_ImplSDL2_NewFrame();
                    ImGui::NewFrame();
                }
            }

            void Render(moth_graphics::graphics::IGraphics& /*graphics*/) override {
                if (m_sdlWindow != nullptr) {
                    ImGui::Render();
                    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
                }
            }

            void DiscardFrame() override {
                if (m_sdlWindow != nullptr) {
                    ImGui::EndFrame();
                }
            }

            void Shutdown() override {
                if (m_sdlWindow != nullptr) {
                    ImGui_ImplSDLRenderer2_Shutdown();
                    ImGui_ImplSDL2_Shutdown();
                    ImGui::DestroyContext();
                    m_sdlWindow = nullptr;
                }
            }

        private:
            SDL_Window* m_sdlWindow = nullptr;
        };
    }

    graphics::Context& Platform::GetGraphicsContext() {
        return *m_context;
    }

    std::unique_ptr<platform::Window> Platform::CreateWindow(std::string_view title, int width, int height) {
        return std::make_unique<platform::sdl::Window>(*m_context, title, width, height);
    }

    bool Platform::Startup() {
        SDL_SetHint(SDL_HINT_VIDEO_ALLOW_SCREENSAVER, "1");
        if (0 > SDL_Init(SDL_INIT_EVERYTHING)) {
            spdlog::error("SDL: initialization failed: {}", SDL_GetError());
            return false;
        }
        spdlog::info("SDL: initialized");
        m_context = std::make_unique<graphics::sdl::Context>();
        if (!m_context->Startup()) {
            spdlog::error("SDL: graphics context startup failed");
            m_context.reset();
            SDL_Quit();
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
        spdlog::info("SDL: shutting down");
        if (m_context) {
            m_context->Shutdown();
            m_context.reset();
        }
        SDL_Quit();
        m_initialized = false;
    }

    std::unique_ptr<moth_graphics::platform::ImGuiContext> Platform::CreateImGuiContext() {
        return std::make_unique<SDLImGuiContext>();
    }
}
