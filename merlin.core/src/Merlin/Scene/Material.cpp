#include "glpch.h"
#include "Material.h"
#include <glm/glm.hpp>

namespace Merlin::Scene {

	GLuint Material::nextID = 0;

    Material::Material(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess)
        : ambient_(ambient), diffuse_(diffuse), specular_(specular), shininess_(shininess) {
        ID = nextID++;
    }

    const glm::vec3& Material::ambient() const { return ambient_; }
    const glm::vec3& Material::diffuse() const { return diffuse_; }
    const glm::vec3& Material::specular() const { return specular_; }
    float Material::shininess() const { return shininess_; }
}