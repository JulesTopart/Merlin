#include "glpch.h"
#include "merlin/graphics/material.h"
#include "merlin/shaders/shader.h"
#include <glm/glm.hpp>

namespace Merlin {

	void PhongMaterial::loadTexture(const std::string& path, TextureType t) {
		Shared<Texture2D> tex = Texture2D::create(path, t);
		tex->setInterpolationMode(GL_LINEAR, GL_LINEAR);
		tex->generateMipmap();
		switch (t) {
		case Merlin::TextureType::DIFFUSE:
			setDiffuseTexture(tex);
			break;
		case Merlin::TextureType::SPECULAR:
			setSpecularTexture(tex);
			break;
		case Merlin::TextureType::NORMAL:
			setNormalTexture(tex);
			break;
		default:
			break;
		}
	}

	void PBRMaterial::loadTexture(const std::string& path, TextureType t) {
		Shared<Texture2D> tex = Texture2D::create(path, t);
		switch (t){
		case Merlin::TextureType::ALBEDO:
			setAlbedoTexture(tex);
			break;
		case Merlin::TextureType::NORMAL:
			setNormalTexture(tex);
			break;
		case Merlin::TextureType::DISPLACMENT:
			//setD
			Console::error("Material") << "DISPLACMENT texture not supported yet" << Console::endl;
			break;
		case Merlin::TextureType::REFLECTION:
			Console::error("Material") << "REFLECTION texture not supported yet" << Console::endl;
			break;
		case Merlin::TextureType::ROUGHNESS:
			setRoughnessTexture(tex);
			break;
		case Merlin::TextureType::METALNESS:
			setMetallicTexture(tex);
			break;
		case Merlin::TextureType::AMBIENT_OCCLUSION:
			setAOTexture(tex);
			break;
		case Merlin::TextureType::MASK:
			Console::error("Material") << "MASK texture not supported yet" << Console::endl;
			break;
		case Merlin::TextureType::EMISSION:
			Console::error("Material") << "EMISSION texture not supported yet" << Console::endl;
			break;
		default:
			break;
		}
	}


	void PhongMaterial::attach(Shader& shader) const {
		if (!shader.supportMaterial())return;
		//shader.setInt("hasMaterial", 1);
		shader.setVec3("material.ambient_color", m_ambient_color);
		shader.setInt("material.use_diffuse_tex", m_diffuse_tex != nullptr && shader.supportTexture());
		if (m_diffuse_tex && shader.supportTexture()) {
			m_diffuse_tex->autoSetUnit();
			m_diffuse_tex->bind();
			m_diffuse_tex->syncTextureUnit(shader, "material.diffuse_tex");
		}
		shader.setVec3("material.diffuse_color", m_diffuse_color);

		shader.setInt("material.use_specular_tex", m_specular_tex != nullptr && shader.supportTexture());
		if (m_specular_tex && shader.supportTexture()) {
			m_specular_tex->autoSetUnit();
			m_specular_tex->bind();
			m_specular_tex->syncTextureUnit(shader, "material.specular_tex");

		}
		shader.setVec3("material.specular_color", m_specular_color);

		shader.setInt("material.use_normal_tex", m_normal_tex != nullptr && shader.supportTexture());
		if (m_normal_tex && shader.supportTexture()) {
			m_normal_tex->autoSetUnit();
			m_normal_tex->bind();
			m_normal_tex->syncTextureUnit(shader, "material.normal_tex");

		}

		shader.setFloat("material.shininess", m_shininess*128);
	}

	void PhongMaterial::detach() const{
		if (m_diffuse_tex) {
			m_diffuse_tex->unbind();
		}

		if (m_normal_tex) {
			m_normal_tex->unbind();
		}

		if (m_specular_tex) {
			m_specular_tex->unbind();
		}

	}


	void PBRMaterial::attach(Shader& shader) const {
		if (!shader.supportMaterial())return;
		shader.setInt("material.use_albedo_tex", m_albedo_tex != nullptr && shader.supportTexture());
		if (m_albedo_tex && shader.supportTexture()) {
			m_albedo_tex->autoSetUnit();
			m_albedo_tex->bind();
			m_albedo_tex->syncTextureUnit(shader, "albedo_tex");

		}
		else shader.setVec3("material.albedo", m_albedo_color);

		shader.setInt("material.use_normal_tex", m_normal_tex != nullptr && shader.supportTexture());
		if (m_normal_tex && shader.supportTexture()) {
			m_normal_tex->autoSetUnit();
			m_normal_tex->bind();
			m_normal_tex->syncTextureUnit(shader, "normal_tex");
		}//use vertex normal otherwise

		shader.setInt("material.use_metallic_tex", m_metalness_tex != nullptr && shader.supportTexture());
		if (m_metalness_tex && shader.supportTexture()) {
			m_metalness_tex->autoSetUnit();
			m_metalness_tex->bind();
			m_metalness_tex->syncTextureUnit(shader, "metalness_tex");
		}
		else shader.setFloat("material.metalness", m_metalness);

		shader.setInt("material.use_roughness_tex", m_roughness_tex != nullptr && shader.supportTexture());
		if (m_roughness_tex && shader.supportTexture()) {
			m_roughness_tex->autoSetUnit();
			m_roughness_tex->bind();
			m_roughness_tex->syncTextureUnit(shader, "roughness_tex");
		}
		else shader.setFloat("material.roughness", m_roughness);

		shader.setInt("material.use_ao_tex", m_ao_tex != nullptr && shader.supportTexture());
		if (m_ao_tex && shader.supportTexture()) {
			m_ao_tex->autoSetUnit();
			m_ao_tex->bind();
			m_ao_tex->syncTextureUnit(shader, "ao_tex");
		}
		else shader.setFloat("material.ao", m_metalness);
	}


	void PBRMaterial::detach() const {
		if (m_albedo_tex) {
			m_albedo_tex->unbind();
		}

		if (m_normal_tex) {
			m_normal_tex->unbind();
		}

		if (m_metalness_tex) {
			m_metalness_tex->unbind();
		}
		
		if (m_roughness_tex) {
			m_roughness_tex->unbind();
		}

		if (m_ao_tex) {
			m_ao_tex->unbind();
		}

	}

	/*
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

	void Material::addTexture(Texture2D_Ptr tex) {
		m_textures[tex->type()] = tex;
	}

	void Material::loadTexture(std::string path, TextureType t) {
		Shared<Texture2D> tex = createShared<Texture2D>(t);
		tex->bind();
		tex->loadFromFile(path);
		tex->setInterpolationMode(GL_LINEAR);
		tex->setRepeatMode(GL_REPEAT);
		addTexture(tex);
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
		case TextureType::ALBEDO:
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
		m_textures[TextureType::ALBEDO] = GenerateDefaultTexture(TextureType::ALBEDO);
		m_textures[TextureType::NORMAL] = GenerateDefaultTexture(TextureType::NORMAL);
		m_textures[TextureType::ROUGHNESS] = GenerateDefaultTexture(TextureType::ROUGHNESS);
		m_textures[TextureType::AMBIENT_OCCLUSION] = GenerateDefaultTexture(TextureType::AMBIENT_OCCLUSION);
		m_textures[TextureType::EMISSION] = GenerateDefaultTexture(TextureType::EMISSION);
	}

	Texture2D& Material::getTexture(TextureType t) const {
		if (m_textures.find(t) != m_textures.end()) return *m_textures.at(t);
		else return *m_textures.at(TextureType::ALBEDO);
	}


	*/

}