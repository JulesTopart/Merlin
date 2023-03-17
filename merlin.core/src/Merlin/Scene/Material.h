#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Graphics/Texture.h"
#include "Merlin/Graphics/Shader.h"
#include "Merlin/Graphics/Mesh.h"
#include <glm/glm.hpp>

namespace Merlin::Graphics {

    class Material {
    public:
        Material();

        const glm::vec3& ambient() const;
        const glm::vec3& diffuse() const;
        const glm::vec3& specular() const;
        const float shininess() const;

        void SetProperty(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess);
        void SetAmbient(const glm::vec3& ambient);
        void SetDiffuse(const glm::vec3& diffuse);
        void SetSpecular(const glm::vec3& specular);
        void SetShininess(const float& shininess);

        
        void LoadShader(std::string vertexPath, std::string fragPath, std::string geomPath = "");
        void SetShader(const Shared<Shader>& shader);

        void LoadTexture(std::string path, TextureType t = TextureType::DIFFUSE, GLuint format = GL_RGB);
        void AddTexture(const Shared<Texture>& tex);

        const Shared<Shader>& GetShader() const;
        const Shared<Texture>& GetTexture(int index) const;
        inline const int GetTextureCount() const { return _textures.size(); };

    private:
        glm::vec3 ambient_ = glm::vec3(0.0);
        glm::vec3 diffuse_ = glm::vec3(0.0);
        glm::vec3 specular_ = glm::vec3(0.0);
        float shininess_ = 0.0;

        Shared<Shader> _shader;
        std::vector<Shared<Texture>> _textures;

        static GLuint nextID;

        GLuint ID;
    };

}
