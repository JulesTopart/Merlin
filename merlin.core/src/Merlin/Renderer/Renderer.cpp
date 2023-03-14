#include "glpch.h"
#include "Merlin/Core/Core.h"
#include "Renderer.h"

namespace Merlin::Renderer {

	Renderer::Renderer() {

	}

	Renderer::~Renderer() {

	}


	void Renderer::SetScene(const Shared<Scene>& scene) {
		_scene = scene;
	}

	void Renderer::SetCamera(const Shared<Camera>& camera) {
		_camera = camera;
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

	void Renderer::Render() {
		glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update the view matrix based on the camera position and orientation
		glm::mat4 view = _camera->GetViewProjectionMatrix();

		// Draw the meshes in the scene
		for (const auto& mesh : _scene->getMeshes()) {
			// Get the material for this mesh
			const Material& material = scene.getMaterial(mesh.materialIndex);

			// Use the shader program for this material
			material.getShader().use();

			// Set the model, view, and projection matrices for the shader
			glm::mat4 modelMatrix = mesh.transform.getMatrix();
			material.getShader().setMat4("uMVPMatrix", mvpMatrix);
			material.getShader().setMat4("uModelMatrix", modelMatrix);

			// Set the texture for the shader
			material.getTexture().bind();

			// Draw the mesh
			mesh.vertexArray.bind();
			glDrawArrays(GL_TRIANGLES, 0, mesh.vertexArray.getNumVertices());
			mesh.vertexArray.unbind();
		}
	}
}