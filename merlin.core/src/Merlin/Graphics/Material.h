#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Memory/Texture.h"
#include "Merlin/Graphics/Shader.h"
#include <glm/glm.hpp>

using namespace Merlin::Memory;

namespace Merlin::Graphics {

    struct MaterialProperty{
        glm::vec3 albedo = glm::vec3(0.0);
        float metallic = 0.0;
        float roughness = 0.0;
        float ao = 0.0;

        glm::vec3 ambient = glm::vec3(0.0);
        glm::vec3 diffuse = glm::vec3(0.0);
        glm::vec3 specular = glm::vec3(0.0);
        float shininess = 0.0;
    };

    class Material{
    public:

        Material(std::string name);

        void SetProperty(const MaterialProperty& props);
        void SetAmbient(const glm::vec3& ambient);
        void SetDiffuse(const glm::vec3& diffuse);
        void SetSpecular(const glm::vec3& specular);
        void SetShininess(const float& shininess);
        void SetAlbedo(const glm::vec3& albedo);
        void SetMetallic(const float& metallic);
        void SetRoughness(const float& roughness);
        void SetAmbientOcclusion(const float& ao);

        void LoadDefaultTexture();
        void LoadTexture(std::string path, TextureType t = TextureType::COLOR);
        void SetTexture(Texture_Ptr tex);
        Texture& GetTexture(TextureType t) const;
        

        inline const glm::vec3& ambient() const { return m_props.ambient; }
        inline const glm::vec3& diffuse() const { return m_props.diffuse; }
        inline const glm::vec3& specular() const { return m_props.specular; }
        inline float shininess() const { return m_props.shininess; }

        inline const glm::vec3& albedo() const { return m_props.albedo; }
        inline float metallic() const { return m_props.metallic; }
        inline float roughness() const { return m_props.roughness; }
        inline float ao() const { return m_props.ao; }

        inline std::string name() { return m_name; }

        inline bool usePBR() const { return m_usePBR; }
        inline void disablePBR() { m_usePBR = false; }
        inline void enablePBR() { m_usePBR = true; }

    private:
        std::string m_name;
        bool m_usePBR = true;
        std::map<TextureType, Texture_Ptr> m_textures;
        MaterialProperty m_props;
    };

    typedef Shared<Material> Material_Ptr;



    class MaterialLibrary {
    public:

        MaterialLibrary();
        void Add(Shared<Material> mat);

        Material& Get(const std::string& name);

        inline unsigned int size() { return m_materials.size(); };
        bool Exists(const std::string& name);
    private:
        void LoadDefaultMaterials();
        std::unordered_map<std::string, Material_Ptr> m_materials;
    };
}
