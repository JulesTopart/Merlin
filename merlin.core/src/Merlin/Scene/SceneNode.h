#pragma once
#include "Merlin/Core/Core.h"

#include "Merlin/Scene/Model.h"
#include "Merlin/Scene/Light.h"

namespace Merlin::Graphics {

    enum class SceneNodeType {
        MODEL, MATERIAL, LIGHT, SCENE, TRANSFORM
    };

	class SceneNode {
	public:
        SceneNode(const std::string& name, SceneNodeType type = SceneNodeType::SCENE);

        virtual void Draw(const Camera& camera) const {};

		void AddChild(Shared<SceneNode> child);
		void RemoveChild(Shared<SceneNode> child);

		bool hasParent() const;
		bool hasChildren() const;
		std::string name() const;
        SceneNodeType type() const;

		std::list<Shared<SceneNode>>& children();
		Shared<SceneNode> parent();

	protected:
		void SetParent(Shared<SceneNode> parent) {
			_parent = parent;
		}

		static int nextID;

        int _ID;
        std::string _name;
        SceneNodeType _type;
		Shared<SceneNode> _parent = nullptr;
		std::list<Shared<SceneNode>> _children;
	};

    //Transform
    class TransformNode : public SceneNode {
    public:
        TransformNode(const std::string& name);

        void SetPosition(const glm::vec3& position);
        void SetRotation(const glm::quat& rotation);
        void SetScale(const glm::vec3& scale);

        glm::vec3 GetPosition() const;
        glm::quat GetRotation() const;
        glm::vec3 GetScale() const;

        glm::mat4 GetModelMatrix() const;

        void Draw(const Camera& camera) const override;

    private:
        glm::vec3 position_;
        glm::quat rotation_;
        glm::vec3 scale_;
    };

    //Model
    class ModelNode : public SceneNode {
    public:
        ModelNode(const std::string& name, const Shared<Model>& model) : SceneNode(name, SceneNodeType::MODEL), model_(model) {}

        void Draw(const Camera& camera) const override {
            model_->Draw(camera);
        }

        const Model& model() const {
            return *model_;
        }

    private:
        Shared<Model> model_;
    };

    //Lights
    class LightNode : public SceneNode {
    public:
        LightNode(const std::string& name, const Shared<Light>& light) : SceneNode(name, SceneNodeType::LIGHT), light_(light) {}

        void Draw(const Camera& camera) const override {
            // Draw light sprite
        }

        const Light& light() const {
            return *light_;
        }

    private:
        Shared<Light> light_;
    };


    //Material
    class MaterialNode : public SceneNode {
    public:
        MaterialNode(const std::string& name, Shared<Material> mat) : SceneNode(name, SceneNodeType::MATERIAL), _material(mat) {}

        void Draw(const Camera& camera) const override {
            // Draw light sprite
        }

        const Material& material() const {
            return *_material;
        }

    private:
        Shared<Material> _material;
    };



}
