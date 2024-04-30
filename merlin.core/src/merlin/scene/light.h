#pragma once
#include "merlin/core/core.h"
#include "merlin/graphics/renderableObject.h"
#include <glm/glm.hpp>

namespace Merlin {

    enum class LightType {
        Directional,
        Point,
        Spot
    };

	class Light : public RenderableObject{
        Light(const std::string name, const glm::vec3& position, const glm::vec3& direction, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, LightType type)
            :RenderableObject(name), position_(position), direction_(direction), ambient_(ambient), diffuse_(diffuse), specular_(specular), type_(type) {}

        const glm::vec3& position() const { return position_; }
        const glm::vec3& direction() const { return direction_; }
        const glm::vec3& ambient() const { return ambient_; }
        const glm::vec3& diffuse() const { return diffuse_; }
        const glm::vec3& specular() const { return specular_; }
        LightType type() const { return type_; }

    private:
        glm::vec3 position_;
        glm::vec3 direction_;
        glm::vec3 ambient_;
        glm::vec3 diffuse_;
        glm::vec3 specular_;
        LightType type_;
	};

}

