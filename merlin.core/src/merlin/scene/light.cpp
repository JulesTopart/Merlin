#include "glpch.h"
#include "light.h"
#include "merlin/core/core.h"

namespace Merlin {

	void Light::draw() const {
		m_mesh->draw();
	}

	void Light::setShadowResolution(GLuint res) {
		m_shadowResolution = res;
	}

	void DirectionalLight::attach(int id, Shader& shader) {
		std::string base = "lights[" + std::to_string(id) + "]";
		shader.setVec3(base + ".ambient", ambient());
		shader.setVec3(base + ".diffuse", diffuse());
		shader.setVec3(base + ".specular", specular());
		shader.setVec3(base + ".attenuation", attenuation());
		shader.setInt(base + ".type", static_cast<int>(type()));
		shader.setInt(base + ".castShadow", m_castShadow);
		shader.setVec3(base + ".direction", glm::vec3(getRenderTransform() * glm::vec4(direction(), 0.0f)));
		if (m_shadowMap && m_castShadow) {
			shader.setMat4(base + ".lightSpaceMatrix", m_lightSpaceMatrix);
			m_shadowMap->autoSetUnit();
			m_shadowMap->bind();
			m_shadowMap->syncTextureUnit(shader, base + ".shadowMap");
		}
	}

	void DirectionalLight::setShadowResolution(GLuint res) {
		m_shadowResolution = res;
		if (m_shadowMap) {
			m_shadowMap->bind();
			m_shadowMap->resize(m_shadowResolution, m_shadowResolution);
			m_shadowMap->unbind();
		}
		if (m_shadowFBO) {
			m_shadowFBO->bind();
			m_shadowFBO->resize(m_shadowResolution, m_shadowResolution);
			m_shadowFBO->unbind();
		}
	}

	void DirectionalLight::detach() {
		if (m_shadowMap) {
			m_shadowMap->unbind();
		}
	}

	void DirectionalLight::attachShadow(Shader& shader) {
		shader.setMat4("lightSpaceMatrix", m_lightSpaceMatrix);
	}

	void DirectionalLight::generateShadowMap() {
		if (!m_shadowMap) {
			m_shadowMap = Texture2D::create(m_shadowResolution, m_shadowResolution, TextureType::SHADOW);
			m_shadowMap->bind();
			m_shadowMap->setInterpolationMode(GL_LINEAR, GL_LINEAR);
			m_shadowMap->setRepeatMode(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
			m_shadowMap->setBorderColor4f(1, 1, 1, 1);
			m_shadowMap->unbind();
		}
		if (!m_shadowFBO) {
			m_shadowFBO = createShared<FBO>(m_shadowResolution, m_shadowResolution);
			m_shadowFBO->bind();
			m_shadowMap->bind();
			m_shadowFBO->attachDepthStencilTexture(m_shadowMap);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			m_shadowFBO->unbind();
			m_shadowMap->unbind();
		}
		float near_plane = 0.1f;
		float far_plane = 15.0f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(glm::vec3(-10) * direction(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		m_lightSpaceMatrix = lightProjection * lightView;
	}

	void PointLight::attach(int id, Shader& shader) {
		std::string base = "lights[" + std::to_string(id) + "]";
		shader.setVec3(base + ".ambient", ambient());
		shader.setVec3(base + ".diffuse", diffuse());
		shader.setVec3(base + ".specular", specular());
		shader.setVec3(base + ".attenuation", attenuation());
		shader.setInt(base + ".type", static_cast<int>(type()));
		shader.setInt(base + ".castShadow", m_castShadow);
		shader.setVec3(base + ".position", glm::vec3(getRenderTransform() * glm::vec4(position(), 1.0f)));

		if (m_shadowMap && m_castShadow) {
			shader.setMat4(base + ".lightSpaceMatrix", m_lightSpaceMatrix);

			m_shadowMap->autoSetUnit();
			m_shadowMap->bind();
			m_shadowMap->syncTextureUnit(shader, base + ".omniShadowMap");
			shader.setFloat(base + ".far_plane", 25.0f);
		}
	}

	void PointLight::setShadowResolution(GLuint res) {
		m_shadowResolution = res;
		if (m_shadowMap) {
			m_shadowMap->bind();
			m_shadowMap->resize(m_shadowResolution, m_shadowResolution);
			m_shadowMap->unbind();
		}
		if (m_shadowFBO) {
			m_shadowFBO->bind();
			m_shadowFBO->resize(m_shadowResolution, m_shadowResolution);
			m_shadowFBO->unbind();
		}
	}

	void PointLight::detach() {
		if (m_shadowMap) {
			m_shadowMap->unbind();
		}
	}


	void PointLight::generateShadowMap() {
		if (!m_shadowMap) {
			m_shadowMap = CubeMap::create(m_shadowResolution, m_shadowResolution, TextureType::SHADOW);
			m_shadowMap->bind();
			m_shadowMap->setInterpolationMode(GL_LINEAR, GL_LINEAR);
			m_shadowMap->setRepeatMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
			m_shadowMap->setBorderColor4f(1, 1, 1, 1);
			m_shadowMap->unbind();
		}
		if (!m_shadowFBO) {
			m_shadowFBO = createShared<FBO>(m_shadowResolution, m_shadowResolution);
			m_shadowFBO->bind();
			m_shadowMap->bind();
			m_shadowFBO->attachDepthStencilTexture(m_shadowMap);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			m_shadowFBO->unbind();
			m_shadowMap->unbind();
		}

		float near_plane = 1.0f;
		float far_plane = 25.0f;
		m_lightSpaceMatrix = glm::perspective(glm::radians(90.0f), 1.0f, near_plane, far_plane);
	}


	glm::mat4 computeViewMAtrix(glm::vec3 position, glm::vec3 direction) {
		glm::vec3 _Front = normalize(direction);
		glm::vec3 _Right = glm::normalize(glm::cross(_Front, glm::vec3(0, 0, 1)));
		glm::vec3 _Up = glm::normalize(glm::cross(_Right, _Front));

		return glm::lookAt(position, position + _Front, _Up);
	}


	void PointLight::attachShadow(Shader& shader) {
		m_shadowTransforms.clear();
		// Correct orientations for each face of the cubemap
		glm::vec3 pos = position();

		m_shadowTransforms.push_back(m_lightSpaceMatrix *
			glm::lookAt(pos, pos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0))); // Positive X
		m_shadowTransforms.push_back(m_lightSpaceMatrix *
			glm::lookAt(pos, pos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0))); // Negative X
		m_shadowTransforms.push_back(m_lightSpaceMatrix *
			glm::lookAt(pos, pos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0))); // Positive Y
		m_shadowTransforms.push_back(m_lightSpaceMatrix *
			glm::lookAt(pos, pos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0))); // Negative Y
		m_shadowTransforms.push_back(m_lightSpaceMatrix *
			glm::lookAt(pos, pos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0))); // Positive Z
		m_shadowTransforms.push_back(m_lightSpaceMatrix *
			glm::lookAt(pos, pos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0))); // Negative Z


		for (int i = 0; i < m_shadowTransforms.size(); i++) {
			shader.setMat4("shadowMatrices[" + std::to_string(i) + "]", m_shadowTransforms[i]);
		}

		shader.setVec3("lightPos", position());
		shader.setFloat("far_plane", 25.0f);
	}
	
	void AmbientLight::attach(int id, Shader& shader) {
		std::string base = "lights[" + std::to_string(id) + "]";
		shader.setVec3(base + ".ambient", ambient());
		shader.setVec3(base + ".diffuse", glm::vec3(0));
		shader.setVec3(base + ".specular", glm::vec3(0));
		//shader.setVec3(base + ".attenuation", attenuation());
		shader.setInt(base + ".type", static_cast<int>(type()));
		//shader.setVec3(base + ".position", glm::vec3(getRenderTransform() * glm::vec4(glm::vec3(0,0,1000000), 1.0f)));
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