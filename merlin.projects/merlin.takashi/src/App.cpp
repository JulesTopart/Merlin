#include "AppLayer.h"

using namespace Merlin;

class App : public Application
{
public:
	App()
		: Application("OpenGL Application")
	{
		pushLayer(new AppLayer());
	}
};

int main()
{
	std::unique_ptr<App> app = std::make_unique<App>();
	app->run();
}