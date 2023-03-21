#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Scene/Model.h"
#include "Merlin/Scene/Light.h"

namespace Merlin::Graphics {

    class SceneObject {
    public:
        virtual ~SceneObject() {}

        virtual void Draw(const Shader& shader, const Camera& camera) const = 0;

        const std::string& name() const {
            return name_;
        }

        inline void SetName(std::string n) { name_ = n; };

    protected:
        std::string name_;
    };


    class ModelObject : public SceneObject {
    public:
        ModelObject(const Shared<Model>& model) : model_(model) {}

        void Draw(const Shader& shader, const Camera& camera) const override {
            model_->Draw(camera);
        }

        const Model& model() const {
            return *model_;
        }

    private:
        Shared<Model> model_;
    };

    class LightObject : public SceneObject {
    public:
        LightObject(const Shared<Light>& light) : light_(light) {}

        void Draw(const Shader& shader, const Camera& camera) const override {
            // Draw light sprite
        }

        const Light& light() const {
            return *light_;
        }

    private:
        Shared<Light> light_;
    };

    class MaterialObject : public SceneObject {
    public:
        MaterialObject(const Shared<Material>& mat) : material_(mat) {}

        void Draw(const Shader& shader, const Camera& camera) const override {
            // Draw light sprite
        }

        const Material& material() const {
            return *material_;
        }

    private:
        Shared<Material> material_;
    };
}