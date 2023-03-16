#include "glpch.h"
#include "Material.h"
#include "Merlin/Renderer/Texture.h"
#include "Merlin/Renderer/Shader.h"
#include <glm/glm.hpp>

namespace Merlin::Renderer {

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


    void Material::SetShader(const Shared<Shader>& shader) {
        if(shader != nullptr)
            _shader = shader;
    }

    void Material::SetTexture(const Shared<Texture>& tex) {
        if (tex != nullptr)
            _texture = tex;
    }

    Shared<Shader> Material::GetShader() {
        return _shader;
    }

    Shared<Texture> Material::GetTexture() {
        return _texture;
    }

    Shader& Material::shader() { return *_shader; }
    Texture& Material::texture() { return *_texture; }
    const glm::vec3& Material::ambient() const { return ambient_; }
    const glm::vec3& Material::diffuse() const { return diffuse_; }
    const glm::vec3& Material::specular() const { return specular_; }
    const float Material::shininess() const { return shininess_; }
}