#include "merlin.h"
#include "AppLayer.h"

using namespace Merlin;

class Example : public Application
{
public:
	Example()
		: Application("OpenGL Examples")
	{
		pushLayer(new AppLayer());
	}
};

int main()
{
	std::unique_ptr<Example> app = std::make_unique<Example>();
	app->run();
}