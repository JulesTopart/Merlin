#include "Templatelayer.h"

using namespace Merlin;

class Template : public Application
{
public:
	Template()
		: Application("OpenGL Application")
	{
		pushLayer(new TemplateLayer());
	}
};

int main()
{
	std::unique_ptr<Template> app = std::make_unique<Template>();
	app->run();
}