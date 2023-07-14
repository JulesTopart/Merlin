#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Memory/Texture.h"
#include "Merlin/Graphics/Shader.h"
#include <glm/glm.hpp>

using namespace Merlin::Memory;

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
       
        void LoadTexture(std::string path, TextureType t = TextureType::COLOR);
        void SetTexture(Shared<Texture> tex);

        Shared<Texture> GetTexture(TextureType t) const;

        inline const glm::vec3& ambient() const { return _props.ambient_; }
        inline const glm::vec3& diffuse() const { return _props.diffuse_; }
        inline const glm::vec3& specular() const { return _props.specular_; }
        inline const float      shininess() const { return _props.shininess_; }
        inline const std::string name() { return _name; }

    private:
        void LoadDefaultTexture();

        MaterialProperty _props;
        std::string _name;
        Shared<Texture> _baseColorTexture;
        Shared<Texture> _roughnessMap;
        Shared<Texture> _normalMap;
        Shared<Texture> _ambientOcclusionMap;
        Shared<Texture> _emissionMap;

    };

    class MaterialLibrary {
    public:

        MaterialLibrary();
        void Add(Shared<Material> mat);
        void LoadDefaultMaterials();
        //std::shared_ptr<Material> Load(const std::string& filepath);
        const Material& Get(const std::string& name);

        inline unsigned int size() { return _materials.size(); };
        bool Exists(const std::string& name);
    private:
        std::unordered_map<std::string, Shared<Material>> _materials;
    };

}
