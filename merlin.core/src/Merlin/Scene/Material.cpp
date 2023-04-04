#include "glpch.h"
#include "Merlin/Scene/Material.h"
#include "Merlin/Memory/Texture.h"
#include "Merlin/Graphics/Shader.h"
#include <glm/glm.hpp>

namespace Merlin::Graphics {
    Material::Material(std::string name) : _name(name){}

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

    void Material::LoadTexture(std::string path, TextureType t, GLuint format) {
        Shared<Texture> tex = CreateShared<Texture>();
        tex->Bind();
        tex->LoadFromFile(path, t, format);
        AddTexture(tex);
    }

    void Material::AddTexture(Shared<Texture> tex) {
        if (tex != nullptr)
            _textures.push_back(tex);
    }

    Shared<Texture> Material::GetTexture(int index) const {
        if (index >= 0 && index < _textures.size() && _textures.size() != 0)
            return _textures[index];
        else Console::error("Material") << "No texture loaded" << Console::endl;
    }


    void MaterialLibrary::Add(Shared<Material> mat){
        if (Exists(mat->name())) Console::error("MaterialLibrary") << "Shader already exists!" << Console::endl;
        else Console::trace("MaterialLibrary") << "Added " << mat->name() << " to the library" << Console::endl;
        _materials[mat->name()] = mat;
    }

    MaterialLibrary::MaterialLibrary(){
        Shared<Material> defaultMaterial = CreateShared<Material>("default");
        defaultMaterial->SetProperty(glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.2, 0.2, 0.2), 0);
        Add(defaultMaterial);
    }

    Shared<Material> MaterialLibrary::Get(const std::string& name){
        if (!Exists(name)) { 
            Console::error("MaterialLibrary") << "Material " << name << " not found ! Using default material instead." << Console::endl;
            return Get("default");
        }
        return _materials[name];
    }

    bool MaterialLibrary::Exists(const std::string& name)
    {
        return _materials.find(name) != _materials.end();
    }


}