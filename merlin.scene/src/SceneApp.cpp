#include "Merlin.h"
#include "ExampleLayer.h"

using namespace Merlin;

class SceneApp : public Application{
public:
	SceneApp()
		: Application("OpenGL Examples")
	{
		PushLayer(new ExampleLayer());
	}
};

int main(){
	std::unique_ptr<SceneApp> app = std::make_unique<SceneApp>();
	app->Run();
}