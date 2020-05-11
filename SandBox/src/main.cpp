#include <Merlin.h>

class SandBox : public Merlin::Application {
public:
	SandBox(){

	}

	~SandBox(){

	}

};

Merlin::Application* Merlin::CreateApplication() {
	return new SandBox;
}