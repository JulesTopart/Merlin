#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Memory/Texture.h"
#include "Merlin/Graphics/Shader.h"
#include "Merlin/Graphics/Mesh.h"
#include <glm/glm.hpp>

namespace Merlin::Graphics {

    struct MaterialProperty{
        glm::vec3 ambient_ = glm::vec3(0.0);
        glm::vec3 diffuse_ = glm::vec3(0.0);
        glm::vec3 specular_ = glm::vec3(0.0);
        float shininess_ = 0.0;
    };

    class Material {
    public:
        Material(std::string name);

        void SetProperty(const MaterialProperty& props);
        void SetProperty(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess);
        void SetAmbient(const glm::vec3& ambient);
        void SetDiffuse(const glm::vec3& diffuse);
        void SetSpecular(const glm::vec3& specular);
        void SetShininess(const float& shininess);
       
        void LoadTexture(std::string path, TextureType t = TextureType::DIFFUSE, GLuint format = GL_RGB);
        void AddTexture(Shared<Texture> tex);

        Shared<Texture> GetTexture(int index = 0) const;
        inline const int GetTextureCount() const { return _textures.size(); }
        
        inline const glm::vec3& ambient() const { return _props.ambient_; }
        inline const glm::vec3& diffuse() const { return _props.diffuse_; }
        inline const glm::vec3& specular() const { return _props.specular_; }
        inline const float      shininess() const { return _props.shininess_; }
        inline const std::string name() { return _name; }

    private:
        MaterialProperty _props;
        std::string _name;
        std::vector<Shared<Texture>> _textures;

    };

    class MaterialLibrary {
    public:

        MaterialLibrary();
        void Add(Shared<Material> mat);
        //std::shared_ptr<Material> Load(const std::string& filepath);
        Shared<Material> Get(const std::string& name);

        inline unsigned int size() { return _materials.size(); };
        bool Exists(const std::string& name);
    private:
        std::unordered_map<std::string, Shared<Material>> _materials;
    };

}
