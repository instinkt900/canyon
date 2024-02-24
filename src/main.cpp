#include "platform/sdl/sdl_app.h"
#include "platform/glfw/glfw_app.h"
#include "layers/layer.h"

class TestLayer : public Layer {
public:
	void Draw() override {

	}
};

int main(int argc, char* argv[]) {
	SDLApplication app{"Testing"};
	//GLFWApplication app{"Testing"};
	app.Run();
	return 0;
}
