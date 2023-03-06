#pragma once
#include "Merlin/Core/Core.h"

#include <glm/glm.hpp>

namespace Merlin::Renderer {

    class Material {
    public:
        Material(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess);

        const glm::vec3& ambient() const;
        const glm::vec3& diffuse() const;
        const glm::vec3& specular() const;
        float shininess() const;

    private:
        glm::vec3 ambient_;
        glm::vec3 diffuse_;
        glm::vec3 specular_;
        float shininess_;

        static GLuint nextID;

        GLuint ID;
    };

}
