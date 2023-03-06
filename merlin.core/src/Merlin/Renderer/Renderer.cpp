#include "glpch.h"
#include "Renderer.h"

namespace Merlin::Renderer {

	Renderer::Renderer() {

	}

	Renderer::~Renderer() {

	}


	void Renderer::SetScene(const Scene& scene) {
		_scene = scene;
	}

	void Renderer::SetCamera(const Camera& camera) {
		_camera = camera;
	}

	void Renderer::Render() {

	}
}