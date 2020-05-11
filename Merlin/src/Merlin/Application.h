#pragma once

#include "Core.h"

namespace Merlin{

	class MERLIN_API Application{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	//To be defined in client
	Application* CreateApplication();

}


