#include "Applayer.h"

using namespace Merlin;

#ifdef __cplusplus
extern "C" {
#endif

	__declspec(dllexport) unsigned long NvOptimusEnablement = 1;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#ifdef __cplusplus
}
#endif

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