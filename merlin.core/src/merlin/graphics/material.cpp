#include "glpch.h"
#include "merlin/graphics/material.h"
#include "merlin/shaders/shader.h"
#include <glm/glm.hpp>

namespace Merlin {
	Material::Material(std::string name) : m_name(name) { loadDefaultTexture(); }

    void Material::setProperty(const MaterialProperty& props) {
		m_props = props;
    }

    void Material::setAmbient(const glm::vec3& ambient) { m_props.ambient = ambient;}
	
    void Material::setDiffuse(const glm::vec3& diffuse) { m_props.diffuse = diffuse; }

    void Material::setSpecular(const glm::vec3& specular) { m_props.specular = specular; }

    void Material::setShininess(const float& shininess) { m_props.shininess = shininess; }

	void Material::setAlbedo(const glm::vec3& albedo) { m_props.albedo = albedo; }

	void Material::setMetallic(const float& metallic) { m_props.metallic = metallic; }

	void Material::setRoughness(const float& roughness) { m_props.roughness = roughness; }

	void Material::setAmbientOcclusion(const float& ao) { m_props.ao = ao; }

	void Material::setTexture(Texture2D_Ptr tex) {
		m_textures[tex->type()] = tex;
	}

	void Material::loadTexture(std::string path, TextureType t) {
		Shared<Texture2D> tex = createShared<Texture2D>(t);
		tex->bind();
		tex->loadFromFile(path);
		tex->setInterpolationMode(GL_LINEAR);
		tex->setRepeatMode(GL_REPEAT);
		setTexture(tex);
	}

	Shared<Texture2D> GenerateDefaultTexture(TextureType t) {

		// create a 1x1 texture with the default color
		Shared<Texture2D> defaultTexture = std::make_shared<Texture2D>(t); // Adapt this line to your Texture class
		defaultTexture->bind();

		ImageData data;
		data.dataType = GL_FLOAT;
		data.height = data.width = 1;
		data.bits = 8;
		data.channels = 3;

		switch (t) {
		case TextureType::COLOR:
			glm::vec3 dColor = glm::vec3(0.5f, 0.5f, 0.5f); // Gray
			data.bytes = &dColor[0];
			defaultTexture->loadFromData(data);
			break;
		case TextureType::ROUGHNESS:
			glm::vec3 dRoughness = glm::vec3(0.04f, 0.04f, 0.04f); // Dark gray
			data.bytes = &dRoughness[0];
			defaultTexture->loadFromData(data);
			break;
		case TextureType::NORMAL:
			glm::vec3 dNormal = glm::vec3(0.5f, 0.5f, 1.0f); // Normal map default (x: 0.5, y: 0.5, z: 1.0)
			data.bytes = &dNormal[0];
			defaultTexture->loadFromData(data);
			break;
		case TextureType::AMBIENT_OCCLUSION:
			glm::vec3 dAO = glm::vec3(1.0f, 1.0f, 1.0f); // White
			data.bytes = &dAO[0];
			defaultTexture->loadFromData(data);
			break;
		case TextureType::EMISSION:
			glm::vec3 dEmission = glm::vec3(0.0f, 0.0f, 0.0f); // Black
			data.bytes = &dEmission[0];
			defaultTexture->loadFromData(data);
			break;
			// Add more cases as needed
		default:
			glm::vec3 defaultData = glm::vec3(0.5f, 0.5f, 0.5f); // Gray
			data.bytes = &defaultData[0];
			defaultTexture->loadFromData(data);
			break;
		}
		
		defaultTexture->setInterpolationMode(GL_LINEAR, GL_LINEAR);
		defaultTexture->setRepeatMode(GL_REPEAT, GL_REPEAT);
		defaultTexture->generateMipmap();

		return defaultTexture;
	}

	void Material::loadDefaultTexture() {
		m_textures[TextureType::COLOR] = GenerateDefaultTexture(TextureType::COLOR);
		m_textures[TextureType::NORMAL] = GenerateDefaultTexture(TextureType::NORMAL);
		m_textures[TextureType::ROUGHNESS] = GenerateDefaultTexture(TextureType::ROUGHNESS);
		m_textures[TextureType::AMBIENT_OCCLUSION] = GenerateDefaultTexture(TextureType::AMBIENT_OCCLUSION);
		m_textures[TextureType::EMISSION] = GenerateDefaultTexture(TextureType::EMISSION);
	}

	Texture2D& Material::getTexture(TextureType t) const {
		if (m_textures.find(t) != m_textures.end()) return *m_textures.at(t);
		else return *m_textures.at(TextureType::COLOR);
	}




}