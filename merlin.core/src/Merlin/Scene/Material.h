#pragma once
#include "Merlin/Core/Core.h"

#include <glm/glm.hpp>

namespace Merlin::Renderer {

    class Material {
    public:
        Material();

        const glm::vec3& ambient() const;
        const glm::vec3& diffuse() const;
        const glm::vec3& specular() const;
        float shininess() const;

        Shader& shader();
        Texture& texture();

        void SetProperty(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess);
        void SetAmbient(const glm::vec3& ambient);
        void SetDiffuse(const glm::vec3& diffuse);
        void SetSpecular(const glm::vec3& specular);
        void SetShininess(const float& shininess);

        void SetShader(const Shared<Shader>&);
        void SetTexture(const Shared<Texture>&);
        Shared<Shader> GetShader();
        Shared<Texture> GetTexture();

    private:
        glm::vec3 ambient_;
        glm::vec3 diffuse_;
        glm::vec3 specular_;
        float shininess_;

        Shared<Shader> _shader;
        Shared<Texture> _texture;

        static GLuint nextID;

        GLuint ID;
    };

}
