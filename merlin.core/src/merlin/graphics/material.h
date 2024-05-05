#pragma once
#include "merlin/core/core.h"
#include "merlin/memory/texture.h"
#include "merlin/shaders/shader.h"
#include <glm/glm.hpp>

#include <map>

namespace Merlin {

    enum class MaterialType {
        PHONG,
        //LAMBERT,
        PBR,
        UNLIT
    };

    class MaterialBase {
    public:
        MaterialBase(std::string name, MaterialType type = MaterialType::UNLIT) : m_type(type), m_name(name) {}

        virtual void attach(Shader& shader) const = 0;
        MaterialType type() const { return m_type; }
        std::string name() const { return m_name; }

    private:
        MaterialType m_type;
        std::string m_name;
    };

    class PhongMaterial : public MaterialBase {
    private:
        Texture2D_Ptr m_diffuse_tex = nullptr;
        Texture2D_Ptr m_specular_tex = nullptr;

        glm::vec3 m_ambient_color;
        glm::vec3 m_diffuse_color;
        glm::vec3 m_specular_color;
        float m_shininess;

    public:
        PhongMaterial(std::string name) : MaterialBase(name, MaterialType::PHONG),  m_ambient_color(0.2f, 0.2f, 0.2f), m_diffuse_color(1.0f, 1.0f, 1.0f), m_specular_color(1.0f, 1.0f, 1.0f), m_shininess(0.2) {}

        inline void setAmbient(const glm::vec3& ambient) { m_ambient_color = ambient; };
        inline void setDiffuse(const glm::vec3& diffuse) { m_diffuse_color = diffuse; };
        inline void setSpecular(const glm::vec3& specular) { m_specular_color = specular; };
        inline void setShininess(const float& shininess) { m_shininess = shininess; };

        inline void setDiffuseTexture(Texture2D_Ptr tex) { m_diffuse_tex = tex; }
        inline void setSpecularTexture(Texture2D_Ptr tex) { m_specular_tex = tex; }

        void attach(Shader& shader) const override {
            if (!shader.supportMaterial())return;
            shader.setInt("material.use_diffuse_tex", m_diffuse_tex != nullptr && shader.supportTexture());
            if (m_diffuse_tex && shader.supportTexture()) {
                m_diffuse_tex->setUnit(TextureBase::getNextTextureUnit());
                m_diffuse_tex->bind();
                m_diffuse_tex->syncTextureUnit(shader, "material.diffuse_tex");
            }
            else shader.setVec3("material.diffuse_color", m_diffuse_color);

            shader.setInt("material.use_specular_tex", m_specular_tex != nullptr && shader.supportTexture());
            if (m_specular_tex && shader.supportTexture()) {
                m_specular_tex->setUnit(TextureBase::getNextTextureUnit());
                m_specular_tex->bind();
                m_specular_tex->syncTextureUnit(shader, "material.specular_tex");
                
            }else shader.setVec3("material.specular_color", m_specular_color);

            shader.setFloat("material.shininess", m_shininess);
        }

    };


    class PBRMaterial : public MaterialBase {
    private:
        Texture2D_Ptr m_albedo_tex;
        Texture2D_Ptr m_normal_tex;
        Texture2D_Ptr m_metalness_tex;
        Texture2D_Ptr m_roughness_tex;
        Texture2D_Ptr m_ao_tex; // Ambient Occlusion Texture

        glm::vec3 m_albedo_color;
        float m_metalness;
        float m_roughness;
        float m_ao;

    public:
        PBRMaterial(std::string name) : MaterialBase(name, MaterialType::PBR), m_albedo_color(1.0f, 1.0f, 1.0f), m_metalness(0.0f), m_roughness(1.0f), m_ao(1.0f) {}

        // Setters for textures
        inline void setAlbedoTexture(Texture2D_Ptr tex) { m_albedo_tex = tex; }
        inline void setNormalTexture(Texture2D_Ptr tex) { m_normal_tex = tex; }
        inline void setMetallicTexture(Texture2D_Ptr tex) { m_metalness_tex = tex; }
        inline void setRoughnessTexture(Texture2D_Ptr tex) { m_roughness_tex = tex; }
        inline void setAOTexture(Texture2D_Ptr tex) { m_ao_tex = tex; }

        void loadTexture(const std::string& path, TextureType t = TextureType::ALBEDO);

        // Setters for material properties
        void setAlbedoColor(const glm::vec3& color) { m_albedo_color = color; }
        void setMetallic(float value) { m_metalness = value; }
        void setRoughness(float value) { m_roughness = value; }
        void setAO(float value) { m_ao = value; }

        void attach(Shader& shader) const override {
            if (!shader.supportMaterial())return;
            shader.setInt("material.use_albedo_tex", m_albedo_tex != nullptr && shader.supportTexture());
            if (m_albedo_tex && shader.supportTexture()) {
                m_albedo_tex->setUnit(TextureBase::getNextTextureUnit());
                m_albedo_tex->bind();
                m_albedo_tex->syncTextureUnit(shader, "albedo_tex");

            }else shader.setVec3("material.albedo", m_albedo_color);
            
            shader.setInt("material.use_normal_tex", m_normal_tex != nullptr && shader.supportTexture());
            if (m_normal_tex && shader.supportTexture()) {
                m_normal_tex->setUnit(TextureBase::getNextTextureUnit());
                m_normal_tex->bind();
                m_normal_tex->syncTextureUnit(shader, "normal_tex");
            }//use vertex normal otherwise

            shader.setInt("material.use_metallic_tex", m_metalness_tex != nullptr && shader.supportTexture());
            if (m_metalness_tex && shader.supportTexture()) {
                m_metalness_tex->setUnit(TextureBase::getNextTextureUnit());
                m_metalness_tex->bind();
                m_metalness_tex->syncTextureUnit(shader, "metalness_tex");
            }else shader.setFloat("material.metalness", m_metalness);
            
            shader.setInt("material.use_roughness_tex", m_roughness_tex != nullptr && shader.supportTexture());
            if (m_roughness_tex && shader.supportTexture()) {
                m_roughness_tex->setUnit(TextureBase::getNextTextureUnit());
                m_roughness_tex->bind();
                m_roughness_tex->syncTextureUnit(shader, "roughness_tex");
            }
            else shader.setFloat("material.roughness", m_roughness);

            shader.setInt("material.use_ao_tex", m_ao_tex != nullptr && shader.supportTexture());
            if (m_ao_tex && shader.supportTexture()) {
                m_ao_tex->setUnit(TextureBase::getNextTextureUnit());
                m_ao_tex->bind();
                m_ao_tex->syncTextureUnit(shader, "ao_tex");
            }
            else shader.setFloat("material.ao", m_metalness);
        }

    };

    /*
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
        
        void setAlbedo(const glm::vec3& albedo);
        void setMetallic(const float& metallic);
        void setRoughness(const float& roughness);
        void setAmbientOcclusion(const float& ao);

        void loadDefaultTexture();
        void loadTexture(std::string path, TextureType t = TextureType::ALBEDO);
        void addTexture(Texture2D_Ptr tex);
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
        std::map<TextureType, Texture2D_Ptr> m_textures;
        MaterialProperty m_props;
    };
    /**/

    typedef Shared<MaterialBase> Material_Ptr;


}
