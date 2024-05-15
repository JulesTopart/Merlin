#pragma once
#include "merlin/core/core.h"
#include "merlin/graphics/renderableObject.h"
#include <glm/glm.hpp>

namespace Merlin {

    enum class LightType {
        Point,
        Directional,
        Spot
    };

	class Light : public RenderableObject{
    public:
        Light(const std::string& name, LightType type, const glm::vec3& ambient = glm::vec3(0.2), const glm::vec3& diffuse = glm::vec3(1), const glm::vec3& specular = glm::vec3(1))
            : RenderableObject(name), ambient_(ambient), diffuse_(diffuse), specular_(specular), type_(type) {}


        virtual void attach(int id, Shader& shader) = 0;
        inline void applyRenderTransform(const glm::mat4& transform) { renderTransform = transform; }
        inline const glm::mat4& getRenderTransform() { return renderTransform; }


        const void setAmbient(const glm::vec3& c) { ambient_ = c; }
        const void setDiffuse(const glm::vec3& c) { diffuse_ = c;}
        const void setSpecular(const glm::vec3& c) { specular_ = c;}

        inline const void setAttenuation(glm::vec3 att) { attenuation_ = att; };
        inline const glm::vec3& attenuation() { return attenuation_; };

        inline const glm::vec3& ambient() const { return ambient_; }
        inline const glm::vec3& diffuse() const { return diffuse_; }
        inline const glm::vec3& specular() const { return specular_; }
        LightType type() const { return type_; }

    private:
        glm::vec3 ambient_;
        glm::vec3 diffuse_;
        glm::vec3 specular_;

        glm::vec3 attenuation_ = glm::vec3(0.01, 0.001f, 0.0001f); //constant, linear, quadratic
        LightType type_;


        glm::mat4 renderTransform = glm::identity<glm::mat4>();
	};

    class DirectionalLight : public Light {
    public:
        DirectionalLight(const std::string& name, const glm::vec3& direction = glm::vec3(0, 0, -1))
            : Light(name, LightType::Directional), direction_(direction) {}
        DirectionalLight(const std::string& name, const glm::vec3& direction, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular)
            : Light(name, LightType::Directional, ambient, diffuse, specular), direction_(direction) {}

        const glm::vec3& direction() const { return direction_; }

        void attach(int id, Shader&) override;

    private:
        glm::vec3 direction_;
    };

    class PointLight : public Light {
    public:

        PointLight(const std::string& name, const glm::vec3& position = glm::vec3(0))
            : Light(name, LightType::Point) {
            setPosition(position);
        }

        PointLight(const std::string& name, const glm::vec3& position, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular)
            : Light(name, LightType::Point, ambient, diffuse, specular) {
            setPosition(position);
        }


        void attach(int id, Shader&) override;

    };

    class SpotLight : public Light {
    public:

        SpotLight(const std::string& name, const glm::vec3& position = glm::vec3(0), const glm::vec3& direction = glm::vec3(0, 0, -1), float cutOff = 50.0f)
            : Light(name, LightType::Spot), direction_(direction), cutOff_(cutOff) {
            setPosition(position);
        }

        SpotLight(const std::string& name, const glm::vec3& position, const glm::vec3& direction, float cutOff, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular)
            : Light(name, LightType::Spot, ambient, diffuse, specular), direction_(direction), cutOff_(cutOff) {
            setPosition(position);
        }

        const glm::vec3& direction() const { return direction_; }
        float cutOff() const { return cutOff_; }

        void attach(int id, Shader&) override;

    private:
        glm::vec3 direction_;

        float cutOff_; // Cut-off angle for the spot light
    };

}

