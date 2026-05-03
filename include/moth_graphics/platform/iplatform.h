#pragma once

#include "moth_graphics/graphics/context.h"

#include <string_view>
#include <memory>

namespace moth_graphics::platform {
    class Window;

    /// @brief Abstract platform backend.
    ///
    /// Responsible for initializing the underlying windowing system (SDL, GLFW,
    /// etc.) and creating windows. Implement this interface to port moth_graphics
    /// to a new platform.
    class IPlatform {
    public:
        virtual ~IPlatform() = default;

        /// @brief Initialize the platform layer.
        /// @returns @c true on success, @c false if initialization failed.
        virtual bool Startup() = 0;

        /// @brief Tear down the platform layer. Called after all windows are destroyed.
        virtual void Shutdown() = 0;

        /// @brief Returns the graphics context owned by this platform.
        virtual moth_graphics::graphics::Context& GetGraphicsContext() = 0;

        /// @brief Create a new window.
        /// @param title Initial window title.
        /// @param width Initial window width in pixels.
        /// @param height Initial window height in pixels.
        virtual std::unique_ptr<Window> CreateWindow(std::string_view title, int width, int height) = 0;
    };
}
