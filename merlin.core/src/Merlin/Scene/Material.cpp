#include "glpch.h"
#include "Merlin/Scene/Material.h"
#include "Merlin/Memory/Texture.h"
#include "Merlin/Graphics/Shader.h"
#include <glm/glm.hpp>

namespace Merlin::Graphics {
    Material::Material(std::string name) : _name(name){ LoadDefaultTexture(); }

    void Material::SetProperty(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess) {
        SetAmbient(ambient);
        SetDiffuse(diffuse);
        SetSpecular(specular);
        SetShininess(shininess);
    }

    void Material::SetProperty(const MaterialProperty& props) {
        _props = props;
    }

    void Material::SetAmbient(const glm::vec3& ambient) { _props.ambient_ = ambient;}

    void Material::SetDiffuse(const glm::vec3& diffuse) { _props.diffuse_ = diffuse; }

    void Material::SetSpecular(const glm::vec3& specular) { _props.specular_ = specular; }

    void Material::SetShininess(const float& shininess) { _props.shininess_ = shininess; }

	void Material::SetTexture(Shared<Texture> tex) {
		switch (tex->type()) {
		case TextureType::COLOR:
			_baseColorTexture = tex;
			break;
		case TextureType::ROUGHNESS:
			_roughnessMap = tex;
			break;
		case TextureType::NORMAL:
			_normalMap = tex;
			break;
		case TextureType::AMBIENT_OCCLUSION:
			_ambientOcclusionMap = tex;
			break;
		case TextureType::EMISSION:
			_emissionMap = tex;
			break;
			// Add more cases as needed
		default:
			break;
		}
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
		_baseColorTexture = GenerateDefaultTexture(TextureType::COLOR);
		_roughnessMap = GenerateDefaultTexture(TextureType::ROUGHNESS);
		_normalMap = GenerateDefaultTexture(TextureType::NORMAL);
		_ambientOcclusionMap = GenerateDefaultTexture(TextureType::AMBIENT_OCCLUSION);
		_emissionMap = GenerateDefaultTexture(TextureType::EMISSION);
	}



	Shared<Texture> Material::GetTexture(TextureType t) const {
		switch (t) {
		case TextureType::COLOR:
			return _baseColorTexture;
		case TextureType::ROUGHNESS:
			return _roughnessMap;
		case TextureType::NORMAL:
			return _normalMap;
		case TextureType::AMBIENT_OCCLUSION:
			return _ambientOcclusionMap;
		case TextureType::EMISSION:
			return _emissionMap;
			// Add more cases as needed
		default:
			return nullptr;
		}
	}



    void MaterialLibrary::Add(Shared<Material> mat){
        if (Exists(mat->name())) Console::error("MaterialLibrary") << "Shader already exists!" << Console::endl;
        else Console::trace("MaterialLibrary") << "Added " << mat->name() << " to the library" << Console::endl;
        _materials[mat->name()] = mat;
    }

    MaterialLibrary::MaterialLibrary(){}

    const Material& MaterialLibrary::Get(const std::string & name) {
        if (!Exists(name)) { 
            Console::error("MaterialLibrary") << "Material " << name << " not found ! Using default material instead." << Console::endl;
            return Get("default");
        }
        return *_materials[name];
    }

    bool MaterialLibrary::Exists(const std::string& name)
    {
        return _materials.find(name) != _materials.end();
    }

    void MaterialLibrary::LoadDefaultMaterials() {
		Shared<Material> defaultMaterial = CreateShared<Material>("default");
		defaultMaterial->SetProperty(glm::vec3(0.6, 0.6, 0.6), glm::vec3(0.4, 0.4, 0.4), glm::vec3(0.2, 0.2, 0.2), 64);
		Add(defaultMaterial);

		Shared<Material> emerald = CreateShared<Material>("emerald");
		emerald->SetAmbient(glm::vec3(0.0215, 0.1745, 0.0215));
		emerald->SetDiffuse(glm::vec3(0.07568, 0.61424, 0.07568));
		emerald->SetSpecular(glm::vec3(0.633, 0.727811, 0.633));
		emerald->SetShininess(0.6);

		Shared<Material> jade = CreateShared<Material>("jade");
		jade->SetAmbient(glm::vec3(0.135, 0.2225, 0.1575));
		jade->SetDiffuse(glm::vec3(0.54, 0.89, 0.63));
		jade->SetSpecular(glm::vec3(0.316228, 0.316228, 0.316228));
		jade->SetShininess(0.1);

		Shared<Material> obsidian = CreateShared<Material>("obsidian");
		obsidian->SetAmbient(glm::vec3(0.05375, 0.05, 0.06625));
		obsidian->SetDiffuse(glm::vec3(0.18275, 0.17, 0.22525));
		obsidian->SetSpecular(glm::vec3(0.332741, 0.328634, 0.346435));
		obsidian->SetShininess(0.3);

		Shared<Material> pearl = CreateShared<Material>("pearl");
		pearl->SetAmbient(glm::vec3(0.25, 0.20725, 0.20725));
		pearl->SetDiffuse(glm::vec3(1, 0.829, 0.829));
		pearl->SetSpecular(glm::vec3(0.296648, 0.296648, 0.296648));
		pearl->SetShininess(0.088);

		Shared<Material> ruby = CreateShared<Material>("ruby");
		ruby->SetAmbient(glm::vec3(0.1745, 0.01175, 0.01175));
		ruby->SetDiffuse(glm::vec3(0.61424, 0.04136, 0.04136));
		ruby->SetSpecular(glm::vec3(0.727811, 0.626959, 0.626959));
		ruby->SetShininess(0.6);

		Shared<Material> turquoise = CreateShared<Material>("turquoise");
		turquoise->SetAmbient(glm::vec3(0.1, 0.18725, 0.1745));
		turquoise->SetDiffuse(glm::vec3(0.396, 0.74151, 0.69102));
		turquoise->SetSpecular(glm::vec3(0.297254, 0.30829, 0.306678));
		turquoise->SetShininess(0.1);

		Shared<Material> brass = CreateShared<Material>("brass");
		brass->SetAmbient(glm::vec3(0.329412, 0.223529, 0.027451));
		brass->SetDiffuse(glm::vec3(0.780392, 0.568627, 0.113725));
		brass->SetSpecular(glm::vec3(0.992157, 0.941176, 0.807843));
		brass->SetShininess(0.21794872);

		Shared<Material> bronze = CreateShared<Material>("bronze");
		bronze->SetAmbient(glm::vec3(0.2125, 0.1275, 0.054));
		bronze->SetDiffuse(glm::vec3(0.714, 0.4284, 0.18144));
		bronze->SetSpecular(glm::vec3(0.393548, 0.271906, 0.166721));
		bronze->SetShininess(0.2);

		Shared<Material> chrome = CreateShared<Material>("chrome");
		chrome->SetAmbient(glm::vec3(0.25, 0.25, 0.25));
		chrome->SetDiffuse(glm::vec3(0.4, 0.4, 0.4));
		chrome->SetSpecular(glm::vec3(0.774597, 0.774597, 0.774597));
		chrome->SetShininess(0.6);

		Shared<Material> copper = CreateShared<Material>("copper");
		copper->SetAmbient(glm::vec3(0.19125, 0.0735, 0.0225));
		copper->SetDiffuse(glm::vec3(0.7038, 0.27048, 0.0828));
		copper->SetSpecular(glm::vec3(0.256777, 0.137622, 0.086014));
		copper->SetShininess(0.1);

		Shared<Material> gold = CreateShared<Material>("gold");
		gold->SetAmbient(glm::vec3(0.24725, 0.1995, 0.0745));
		gold->SetDiffuse(glm::vec3(0.75164, 0.60648, 0.22648));
		gold->SetSpecular(glm::vec3(0.628281, 0.555802, 0.366065));
		gold->SetShininess(0.4);

		Shared<Material> silver = CreateShared<Material>("silver");
		silver->SetAmbient(glm::vec3(0.19225, 0.19225, 0.19225));
		silver->SetDiffuse(glm::vec3(0.50754, 0.50754, 0.50754));
		silver->SetSpecular(glm::vec3(0.508273, 0.508273, 0.508273));
		silver->SetShininess(0.4);

		Shared<Material> black_plastic = CreateShared<Material>("black plastic");
		black_plastic->SetAmbient(glm::vec3(0.0, 0.0, 0.0));
		black_plastic->SetDiffuse(glm::vec3(0.01, 0.01, 0.01));
		black_plastic->SetSpecular(glm::vec3(0.50, 0.50, 0.50));
		black_plastic->SetShininess(0.25);

		Shared<Material> cyan_plastic = CreateShared<Material>("cyan plastic");
		cyan_plastic->SetAmbient(glm::vec3(0.0, 0.1, 0.06));
		cyan_plastic->SetDiffuse(glm::vec3(0.0, 0.50980392, 0.50980392));
		cyan_plastic->SetSpecular(glm::vec3(0.50196078, 0.50196078, 0.50196078));
		cyan_plastic->SetShininess(0.25);

		Shared<Material> green_plastic = CreateShared<Material>("green plastic");
		green_plastic->SetAmbient(glm::vec3(0.0, 0.0, 0.0));
		green_plastic->SetDiffuse(glm::vec3(0.1, 0.35, 0.1));
		green_plastic->SetSpecular(glm::vec3(0.45, 0.55, 0.45));
		green_plastic->SetShininess(0.25);

		Shared<Material> red_plastic = CreateShared<Material>("red plastic");
		red_plastic->SetAmbient(glm::vec3(0.0, 0.0, 0.0));
		red_plastic->SetDiffuse(glm::vec3(0.5, 0.0, 0.0));
		red_plastic->SetSpecular(glm::vec3(0.7, 0.6, 0.6));
		red_plastic->SetShininess(0.25);

		Shared<Material> white_plastic = CreateShared<Material>("white plastic");
		white_plastic->SetAmbient(glm::vec3(0.0, 0.0, 0.0));
		white_plastic->SetDiffuse(glm::vec3(0.55, 0.55, 0.55));
		white_plastic->SetSpecular(glm::vec3(0.70, 0.70, 0.70));
		white_plastic->SetShininess(0.25);

		Shared<Material> yellow_plastic = CreateShared<Material>("yellow plastic");
		yellow_plastic->SetAmbient(glm::vec3(0.0, 0.0, 0.0));
		yellow_plastic->SetDiffuse(glm::vec3(0.5, 0.5, 0.0));
		yellow_plastic->SetSpecular(glm::vec3(0.60, 0.60, 0.50));
		yellow_plastic->SetShininess(0.25);

		Shared<Material> black_rubber = CreateShared<Material>("black rubber");
		black_rubber->SetAmbient(glm::vec3(0.02, 0.02, 0.02));
		black_rubber->SetDiffuse(glm::vec3(0.01, 0.01, 0.01));
		black_rubber->SetSpecular(glm::vec3(0.4, 0.4, 0.4));
		black_rubber->SetShininess(0.078125);

		Shared<Material> cyan_rubber = CreateShared<Material>("cyan rubber");
		cyan_rubber->SetAmbient(glm::vec3(0.0, 0.05, 0.05));
		cyan_rubber->SetDiffuse(glm::vec3(0.4, 0.5, 0.5));
		cyan_rubber->SetSpecular(glm::vec3(0.04, 0.7, 0.7));
		cyan_rubber->SetShininess(0.078125);

		Shared<Material> green_rubber = CreateShared<Material>("green rubber");
		green_rubber->SetAmbient(glm::vec3(0.0, 0.05, 0.0));
		green_rubber->SetDiffuse(glm::vec3(0.4, 0.5, 0.4));
		green_rubber->SetSpecular(glm::vec3(0.04, 0.7, 0.04));
		green_rubber->SetShininess(0.078125);

		Shared<Material> red_rubber = CreateShared<Material>("red rubber");
		red_rubber->SetAmbient(glm::vec3(0.05, 0.0, 0.0));
		red_rubber->SetDiffuse(glm::vec3(0.5, 0.4, 0.4));
		red_rubber->SetSpecular(glm::vec3(0.7, 0.04, 0.04));
		red_rubber->SetShininess(0.078125);

		Shared<Material> white_rubber = CreateShared<Material>("white rubber");
		white_rubber->SetAmbient(glm::vec3(0.05, 0.05, 0.05));
		white_rubber->SetDiffuse(glm::vec3(0.5, 0.5, 0.5));
		white_rubber->SetSpecular(glm::vec3(0.7, 0.7, 0.7));
		white_rubber->SetShininess(0.078125);

		Shared<Material> yellow_rubber = CreateShared<Material>("yellow rubber");
		yellow_rubber->SetAmbient(glm::vec3(0.05, 0.05, 0.0));
		yellow_rubber->SetDiffuse(glm::vec3(0.5, 0.5, 0.4));
		yellow_rubber->SetSpecular(glm::vec3(0.7, 0.7, 0.04));
		yellow_rubber->SetShininess(0.078125);

		Add(emerald);
		Add(jade);
		Add(obsidian);
		Add(pearl);
		Add(ruby);
		Add(turquoise);
		Add(brass);
		Add(bronze);
		Add(chrome);
		Add(copper);
		Add(gold);
		Add(silver);
		Add(black_plastic);
		Add(cyan_plastic);
		Add(green_plastic);
		Add(red_plastic);
		Add(white_plastic);
		Add(yellow_plastic);
		Add(black_rubber);
		Add(cyan_rubber);
		Add(green_rubber);
		Add(red_rubber);
		Add(white_rubber);
		Add(yellow_rubber);

    }


}