#include "glpch.h"
#include "Merlin/Scene/Material.h"
#include "Merlin/Graphics/Texture.h"
#include "Merlin/Graphics/Shader.h"
#include <glm/glm.hpp>

namespace Merlin::Graphics {

	GLuint Material::nextID = 0;

    Material::Material(){
        ID = nextID++;
    }

    void Material::SetProperty(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess) {
        SetAmbient(ambient);
        SetDiffuse(diffuse);
        SetSpecular(specular);
        SetShininess(shininess);
    }

    void Material::SetAmbient(const glm::vec3& ambient) { ambient_ = ambient;}

    void Material::SetDiffuse(const glm::vec3& diffuse) { diffuse_ = diffuse; }

    void Material::SetSpecular(const glm::vec3& specular) { specular_ = specular; }

    void Material::SetShininess(const float& shininess) { shininess_ = shininess; }


    void Material::LoadShader(std::string vertexPath, std::string fragPath, std::string geomPath) {
        Shared<Shader> sh = CreateShared<Shader>(vertexPath.substr(0, vertexPath.find(".vert")));
        sh->Compile(vertexPath, fragPath, geomPath);
    }

    void Material::SetShader(const Shared<Shader>& shader) {
        if(shader != nullptr)
            _shader = shader;
    }


    void Material::LoadTexture(std::string path, TextureType t, GLuint format) {
        Shared<Texture> tex = CreateShared<Texture>();
        tex->Bind();
        tex->LoadFromFile(path, t, format);
        AddTexture(tex);
    }


    void Material::AddTexture(const Shared<Texture>& tex) {
        if (tex != nullptr)
            _textures.push_back(tex);
    }

    const Shared<Shader>& Material::GetShader() const {
        return _shader;
    }

    const Shared<Texture>& Material::GetTexture(int index) const {
        if(index > 0 && index < _textures.size())
            return _textures[index];
    }

    const glm::vec3& Material::ambient() const { return ambient_; }
    const glm::vec3& Material::diffuse() const { return diffuse_; }
    const glm::vec3& Material::specular() const { return specular_; }
    const float Material::shininess() const { return shininess_; }
}