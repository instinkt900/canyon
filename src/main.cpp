#include "canyon.h"
#include "layers/layer.h"
#include "platform/vulkan_application.h"
#include "platform/sdl_application.h"

class TestLayer : public Layer {
public:
    TestLayer(graphics::IGraphics& graphics)
        : m_graphics(graphics) {
    }

    void Draw() override {
        m_graphics.SetColor(graphics::Color(1.0f, 1.0f, 1.0f, 1.0f));
        m_graphics.Clear();

        m_graphics.SetColor(graphics::Color(1.0f, 0, 0, 1.0f));
        m_graphics.DrawFillRectF(MakeRect(10.0f,10.0f,GetWidth() - 20.0f,GetHeight() - 20.0f));
    }

    graphics::IGraphics& m_graphics;
};

int main(int argc, char* argv[]) {
    VulkApplication app{};
    //SDLApplication app{};
    app.GetLayerStack().PushLayer(std::make_unique<TestLayer>(app.GetGraphics()));
    app.TickSync();
    return 0;
}
