#include "Merlin.h"
#include "ExampleLayer.h"

using namespace Merlin;

class Example : public Application
{
public:
	Example()
		: Application("Merlin sandbox", 1280,720, true, true)
	{
		PushLayer(new ExampleLayer());
	}
};

int main()
{
	std::unique_ptr<Example> app = std::make_unique<Example>();
	app->Run();
}