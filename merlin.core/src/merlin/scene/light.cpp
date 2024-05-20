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

		if (m_shadowMap) {
			shader.setMat4(base + ".lightSpaceMatrix", m_lightSpaceMatrix);
			m_shadowMap->setUnit(TextureBase::getNextTextureUnit());
			m_shadowMap->bind();
			m_shadowMap->syncTextureUnit(shader, base + ".shadowMap");
		}
	}

	void DirectionalLight::attachShadow(Shader& shader) {
		shader.setMat4("lightSpaceMatrix", m_lightSpaceMatrix);
	}

	void DirectionalLight::generateShadowMap(){
		if (!m_shadowMap) {
			m_shadowMap = Texture2D::create(2048, 2048, TextureType::SHADOW);
			m_shadowMap->bind();
			m_shadowMap->setInterpolationMode(GL_LINEAR, GL_LINEAR);
			m_shadowMap->setRepeatMode(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
			m_shadowMap->setBorderColor4f(1, 1, 1, 1);
			m_shadowMap->unbind();
		}
		if (!m_shadowFBO) {
			m_shadowFBO = createShared<FBO>(2048, 2048);
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

		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(m_lightSpaceMatrix *
			glm::lookAt(position(), position() + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(m_lightSpaceMatrix *
			glm::lookAt(position(), position() + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(m_lightSpaceMatrix *
			glm::lookAt(position(), position() + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		shadowTransforms.push_back(m_lightSpaceMatrix *
			glm::lookAt(position(), position() + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
		shadowTransforms.push_back(m_lightSpaceMatrix *
			glm::lookAt(position(), position() + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(m_lightSpaceMatrix *
			glm::lookAt(position(), position() + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
	}

	void PointLight::attach(int id, Shader& shader) {
		std::string base = "lights[" + std::to_string(id) + "]";
		shader.setVec3(base + ".ambient", ambient());
		shader.setVec3(base + ".diffuse", diffuse());
		shader.setVec3(base + ".specular", specular());
		shader.setVec3(base + ".attenuation", attenuation());
		shader.setInt(base + ".type", static_cast<int>(type()));
		shader.setVec3(base + ".position", glm::vec3(getRenderTransform() * glm::vec4(position(), 1.0f)));

		
		if (m_shadowMap) {
			//shader.setMat4(base + ".lightSpaceMatrix", m_lightSpaceMatrix);
			m_shadowMap->setUnit(0);
			m_shadowMap->bind();
			m_shadowMap->syncTextureUnit(shader, base + ".omniShadowMap");
			shader.setFloat(base + ".far_plane", 100.0);
		}
	}

	void PointLight::attachShadow(Shader& shader) {
		//shader.setMat4("lightSpaceMatrix", m_lightSpaceMatrix);
		for(int i = 0; i < m_shadowTransforms.size(); i++)
			shader.setMat4("shadowMatrices[" + std::to_string(i) + "]", m_shadowTransforms[i]);

		shader.setVec3("lightPos", position());
		shader.setFloat("far_plane", 100.0);
	}

	void PointLight::generateShadowMap() {
		if (!m_shadowMap) {
			m_shadowMap = CubeMap::create(2048, 2048, TextureType::SHADOW);
			m_shadowMap->bind();
			m_shadowMap->setInterpolationMode(GL_LINEAR, GL_LINEAR);
			m_shadowMap->setRepeatMode(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
			m_shadowMap->setBorderColor4f(1, 1, 1, 1);
			m_shadowMap->unbind();
		}
		if (!m_shadowFBO) {
			m_shadowFBO = createShared<FBO>(2048, 2048);
			m_shadowFBO->bind();
			m_shadowMap->bind();
			m_shadowFBO->attachDepthStencilTexture(m_shadowMap);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			m_shadowFBO->unbind();
			m_shadowMap->unbind();
		}

		float aspect = 1.0;
		float near_plane = 0.1f;
		float far_plane = 15.0f;
		m_lightSpaceMatrix = glm::perspective(glm::radians(90.0f), aspect, near_plane, far_plane);
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