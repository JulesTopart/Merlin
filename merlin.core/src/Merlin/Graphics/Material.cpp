#include "glpch.h"
#include "Merlin/Graphics/Material.h"

#include "Merlin/Graphics/Shader.h"
#include <glm/glm.hpp>

namespace Merlin::Graphics {
	Material::Material(std::string name) : m_name(name) { LoadDefaultTexture(); }

    void Material::SetProperty(const MaterialProperty& props) {
		m_props = props;
    }

    void Material::SetAmbient(const glm::vec3& ambient) { m_props.ambient = ambient;}
	
    void Material::SetDiffuse(const glm::vec3& diffuse) { m_props.diffuse = diffuse; }

    void Material::SetSpecular(const glm::vec3& specular) { m_props.specular = specular; }

    void Material::SetShininess(const float& shininess) { m_props.shininess = shininess; }

	void Material::SetAlbedo(const glm::vec3& albedo) { m_props.albedo = albedo; }

	void Material::SetMetallic(const float& metallic) { m_props.metallic = metallic; }

	void Material::SetRoughness(const float& roughness) { m_props.roughness = roughness; }

	void Material::SetAmbientOcclusion(const float& ao) { m_props.ao = ao; }

	void Material::SetTexture(Texture_Ptr tex) {
		m_textures[tex->type()] = tex;
	}

	void Material::LoadTexture(std::string path, TextureType t) {
		Shared<Texture> tex = CreateShared<Texture>(t);
		tex->Bind();
		tex->LoadFromFile(path);
		tex->SetInterpolationMode(GL_LINEAR);
		tex->SetRepeatMode(GL_REPEAT);
		SetTexture(tex);
	}

	Shared<Texture> GenerateDefaultTexture(TextureType t) {
		// Determine the default color for the texture type
		glm::vec3 defaultColor;
		switch (t) {
		case TextureType::COLOR:
			defaultColor = glm::vec3(0.5f, 0.5f, 0.5f); // Gray
			break;
		case TextureType::ROUGHNESS:
			defaultColor = glm::vec3(0.04f, 0.04f, 0.04f); // Dark gray
			break;
		case TextureType::NORMAL:
			defaultColor = glm::vec3(0.5f, 0.5f, 1.0f); // Normal map default (x: 0.5, y: 0.5, z: 1.0)
			break;
		case TextureType::AMBIENT_OCCLUSION:
			defaultColor = glm::vec3(1.0f, 1.0f, 1.0f); // White
			break;
		case TextureType::EMISSION:
			defaultColor = glm::vec3(0.0f, 0.0f, 0.0f); // Black
			break;
			// Add more cases as needed
		default:
			defaultColor = glm::vec3(0.5f, 0.5f, 0.5f); // Gray
			break;
		}

		// Create a 1x1 texture with the default color
		Shared<Texture> defaultTexture = std::make_shared<Texture>(t); // Adapt this line to your Texture class
		defaultTexture->Bind();
		defaultTexture->Allocate(1, 1, GL_RGB);
		defaultTexture->LoadFromData((unsigned char*)&defaultColor[0], 1, 1, GL_RGB);
		defaultTexture->SetInterpolationMode(GL_LINEAR, GL_LINEAR);
		defaultTexture->SetRepeatMode(GL_REPEAT, GL_REPEAT);
		defaultTexture->GenerateMipMap();

		return defaultTexture;
	}

	void Material::LoadDefaultTexture() {
		m_textures[TextureType::COLOR] = GenerateDefaultTexture(TextureType::COLOR);
		m_textures[TextureType::NORMAL] = GenerateDefaultTexture(TextureType::NORMAL);
		m_textures[TextureType::ROUGHNESS] = GenerateDefaultTexture(TextureType::ROUGHNESS);
		m_textures[TextureType::AMBIENT_OCCLUSION] = GenerateDefaultTexture(TextureType::AMBIENT_OCCLUSION);
		m_textures[TextureType::EMISSION] = GenerateDefaultTexture(TextureType::EMISSION);
	}

	Texture& Material::GetTexture(TextureType t) const {
		if (m_textures.find(t) != m_textures.end()) return *m_textures.at(t);
		else return *m_textures.at(TextureType::COLOR);
	}




}