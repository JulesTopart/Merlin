#include "Merlin.h"
#include "ExampleLayer.h"

using namespace Merlin;

class RaytracingApp : public Application
{
public:
	RaytracingApp()
		: Application("OpenGL Examples")
	{
		PushLayer(new ExampleLayer());
	}
};

int main()
{
	std::unique_ptr<RaytracingApp> app = std::make_unique<RaytracingApp>();
	app->Run();
}