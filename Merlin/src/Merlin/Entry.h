#pragma once
#include "Application.h"

#ifdef  MRLN_PLATFORM_WINDOWS

extern Merlin::Application* Merlin::CreateApplication();

int main(int argc, char** argv) {
	printf("Merlin engine");
	auto app = Merlin::CreateApplication();
	app->Run();
	delete app;
}

#endif
