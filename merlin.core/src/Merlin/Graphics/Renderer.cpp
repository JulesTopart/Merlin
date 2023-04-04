#include "glpch.h"
#include "Merlin/Core/Core.h"
#include "Renderer.h"

namespace Merlin::Graphics {

	Renderer::Renderer() {
		backgroundColor = glm::vec4(0.2f, 0.3f, 0.4f, 1.0f);

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

	void Renderer::Render(const Model& model, const Camera& camera) {
		// Update the view matrix based on the camera position and orientation
		glm::mat4 view = camera.GetViewProjectionMatrix();
		std::vector<std::string> shadersNames;

		for (int index = 0; index < model.meshes().size(); index++) {
			shadersNames.push_back()
		}
	}

	void Render(const Model& model, const Camera& camera) {

	}

	void Render(const Mesh& mesh, const Camera& camera) {

	}


	const Material& Renderer::GetMaterial(std::string n) {
		return *_materialLibrary.Get(n);
	}

	const Shader& Renderer::GetShader(std::string n) {
		return *_shaderLibrary.Get(n);
	}

	void Renderer::LoadShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::string& geomShaderPath = "") {
		Shared<Shader> shader = CreateShared<Shader>(vertexShaderPath, fragmentShaderPath, geomShaderPath);
		_shaderLibrary.Add(shader);
	}

	void Renderer::CreateMaterial(MaterialProperty matProps){
		std::string name = "material";
		std::stringstream ss;
		ss << name << _materialLibrary.size();
		Shared<Material> mat = CreateShared<Material>(ss.str());
		_materialLibrary.Add(mat);
	}

	void Renderer::AddMaterial(Shared<Material> material) {
		_materialLibrary.Add(material);
	}

	void Renderer::AddShader(Shared<Shader> shader) {
		_shaderLibrary.Add(shader);
	}

	void Renderer::Clear() {
		glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer::SetBackgroundColor(float r, float g, float b, float a) {
		backgroundColor = glm::vec4(r, g, b, a);
	}



}