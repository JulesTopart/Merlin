#include "glpch.h"
#include "light.h"

namespace Merlin {

	void Light::draw() const {
		m_mesh->draw();
	}

	void DirectionalLight::attach(int id, Shader& shader) {
		std::string base = "lights[" + std::to_string(id) + "]";
		shader.setVec3(base + ".ambient", ambient());
		shader.setVec3(base + ".diffuse", diffuse());
		shader.setVec3(base + ".specular", specular());
		shader.setVec3(base + ".attenuation", attenuation());
		shader.setInt(base + ".type", static_cast<int>(type()));
		shader.setVec3(base + ".direction", glm::vec3(getRenderTransform() * glm::vec4(direction(), 0.0f)));
	}

	void PointLight::attach(int id, Shader& shader) {
		std::string base = "lights[" + std::to_string(id) + "]";
		shader.setVec3(base + ".ambient", ambient());
		shader.setVec3(base + ".diffuse", diffuse());
		shader.setVec3(base + ".specular", specular());
		shader.setVec3(base + ".attenuation", attenuation());
		shader.setInt(base + ".type", static_cast<int>(type()));
		shader.setVec3(base + ".position", glm::vec3(getRenderTransform() * glm::vec4(position(), 1.0f)));
	}

	void AmbientLight::attach(int id, Shader& shader) {
		std::string base = "lights[" + std::to_string(id) + "]";
		shader.setVec3(base + ".ambient", ambient());
		shader.setVec3(base + ".diffuse", glm::vec3(0));
		shader.setVec3(base + ".specular", glm::vec3(0));
		shader.setVec3(base + ".attenuation", attenuation());
		shader.setInt(base + ".type", static_cast<int>(type()));
		shader.setVec3(base + ".position", glm::vec3(getRenderTransform() * glm::vec4(glm::vec3(0,0,1000000), 1.0f)));
	}

	void SpotLight::attach(int id, Shader& shader) {
		std::string base = "lights[" + std::to_string(id) + "]";
		shader.setVec3(base + ".ambient", ambient());
		shader.setVec3(base + ".diffuse", diffuse());
		shader.setVec3(base + ".specular", specular());
		shader.setVec3(base + ".attenuation", attenuation());
		shader.setInt(base + ".type", static_cast<int>(type()));
		shader.setVec3(base + ".position", glm::vec3(getRenderTransform() * glm::vec4(position(), 1.0f))); //point
		shader.setVec3(base + ".direction", glm::vec3(getRenderTransform() * glm::vec4(direction(), 0.0f))); //pure vector
		shader.setFloat(base + ".cutOff", cutOff());
	}
}