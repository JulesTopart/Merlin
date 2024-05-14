#include "glpch.h"
#include "light.h"

namespace Merlin {
	void DirectionalLight::attach(int id, Shader& shader) {
		std::string base = "lights[" + std::to_string(id) + "]";
		shader.setVec3(base + ".ambient", ambient());
		shader.setVec3(base + ".diffuse", diffuse());
		shader.setVec3(base + ".specular", specular());
		shader.setInt(base + ".type", static_cast<int>(type()));
		shader.setVec3(base + ".direction", glm::vec3(getRenderTransform() * glm::vec4(direction(), 1.0f)));
	}

	void PointLight::attach(int id, Shader& shader) {
		std::string base = "lights[" + std::to_string(id) + "]";
		shader.setVec3(base + ".ambient", ambient());
		shader.setVec3(base + ".diffuse", diffuse());
		shader.setVec3(base + ".specular", specular());
		shader.setInt(base + ".type", static_cast<int>(type()));
		shader.setVec3(base + ".position", glm::vec3(getRenderTransform() * glm::vec4(position(), 1.0f)));
	}

	void SpotLight::attach(int id, Shader& shader) {
		std::string base = "lights[" + std::to_string(id) + "]";
		shader.setVec3(base + ".ambient", ambient());
		shader.setVec3(base + ".diffuse", diffuse());
		shader.setVec3(base + ".specular", specular());
		shader.setInt(base + ".type", static_cast<int>(type()));
		shader.setVec3(base + ".position", glm::vec3(getRenderTransform() * glm::vec4(position(), 1.0f)));
		shader.setVec3(base + ".direction", glm::vec3(getRenderTransform() * glm::vec4(direction(), 1.0f)));
		shader.setFloat(base + ".cutOff", cutOff());
	}
} 