#pragma once
#include "merlin/core/core.h"
#include "merlin/memory/texture.h"
#include "merlin/shaders/shader.h"
#include <glm/glm.hpp>

#include <map>

namespace Merlin {

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

        void setProperty(const MaterialProperty& props);
        void setAmbient(const glm::vec3& ambient);
        void setDiffuse(const glm::vec3& diffuse);
        void setSpecular(const glm::vec3& specular);
        void setShininess(const float& shininess);
        void setAlbedo(const glm::vec3& albedo);
        void setMetallic(const float& metallic);
        void setRoughness(const float& roughness);
        void setAmbientOcclusion(const float& ao);

        void loadDefaultTexture();
        void loadTexture(std::string path, TextureType t = TextureType::COLOR);
        void setTexture(Texture_Ptr tex);
        Texture2D& getTexture(TextureType t) const;
        

        inline const glm::vec3& ambient() const { return m_props.ambient; }
        inline const glm::vec3& diffuse() const { return m_props.diffuse; }
        inline const glm::vec3& specular() const { return m_props.specular; }
        inline float shininess() const { return m_props.shininess; }

        inline const glm::vec3& albedo() const { return m_props.albedo; }
        inline float metallic() const { return m_props.metallic; }
        inline float roughness() const { return m_props.roughness; }
        inline float ao() const { return m_props.ao; }

        inline std::string name() { return m_name; }

    private:
        std::string m_name;
        std::map<TextureType, Texture_Ptr> m_textures;
        MaterialProperty m_props;
    };

    typedef Shared<Material> Material_Ptr;


}
