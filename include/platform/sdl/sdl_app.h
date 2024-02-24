#pragma once

#include "application.h"
#include <SDL.h>
#include <string>

class SDLApplication : public Application {
public:
    SDLApplication(std::string const& applicationTitle);
    virtual ~SDLApplication();

    void SetWindowTitle(std::string const& title) override;

protected:
    bool CreateWindow() override;
    void DestroyWindow() override;
    void SetupLayers() override;
    void UpdateWindow() override;
    void Draw() override;

private:
    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;
};
