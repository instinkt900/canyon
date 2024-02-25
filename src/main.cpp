#include "platform/sdl/sdl_app.h"
#include "platform/glfw/glfw_app.h"
#include "layers/layer.h"
#include "graphics/sdl/sdl_graphics.h"

using namespace graphics::sdl;

class TestLayer : public Layer {
public:
    TestLayer(SDLGraphics* graphics)
        :m_graphics(graphics) {
    }

	void Draw() override {
        m_graphics->SetColor(graphics::Color(1.0f,1.0f,1.0f,1.0f));
        m_graphics->Clear();
	}

    graphics::sdl::SDLGraphics* m_graphics;
};

class TestApplication : public SDLApplication {
public:
    TestApplication()
        :SDLApplication("Testing") {
    }

    bool CreateWindow() override {
        if (!SDLApplication::CreateWindow()) {
            return false;
        }
        m_graphics = std::make_unique<SDLGraphics>(GetSDLRenderer());
        return true;
    }

    void SetupLayers() override {
        SDLApplication::SetupLayers();
        std::unique_ptr<TestLayer> layer(std::make_unique<TestLayer>(m_graphics.get()));
        m_layerStack->PushLayer(std::move(layer));
    }

    std::unique_ptr<SDLGraphics> m_graphics;
};

int main(int argc, char* argv[]) {
	SDLApplication app{"Testing"};
	//GLFWApplication app{"Testing"};
	app.Run();
	return 0;
}

