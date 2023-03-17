#include "glpch.h"
#include "Merlin/Core/Core.h"
#include "Renderer.h"

namespace Merlin::Graphics {

	Renderer::Renderer() {

	}

	Renderer::~Renderer() {

	}

	void Renderer::DisableMultisampling() {
		glDisable(GL_MULTISAMPLE);
	}

	void Renderer::DisableTransparency() {
		glDisable(GL_BLEND);
	}

	void Renderer::EnableTransparency() {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void Renderer::EnableMultisampling() {
		glEnable(GL_MULTISAMPLE);
	}

	void Renderer::Initialize() {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);


	}

	void Renderer::Render(const Shared<Scene>& scene, const Shared<Camera>& camera) {
		glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update the view matrix based on the camera position and orientation
		glm::mat4 view = camera->GetViewProjectionMatrix();

		// Draw the meshes in the scene
		for (const auto& model : scene->GetModels()) {
			model->Draw(view);
		}
	}
}