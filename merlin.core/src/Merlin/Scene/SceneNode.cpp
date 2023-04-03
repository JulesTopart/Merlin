#include "glpch.h"
#include "SceneNode.h"

namespace Merlin::Graphics {

	int SceneNode::nextID = 0;

	SceneNode::SceneNode(const std::string& name, SceneNodeType type) : _name(name), _type(type) {
		_ID = nextID++;
	}

	void SceneNode::AddChild(Shared<SceneNode> child) {
		if (child != nullptr)
			_children.push_back(child);
		else Console::warn("SceneNode") << "Empty node child ignored" << Console::endl;
	}
	void SceneNode::RemoveChild(Shared<SceneNode> child) {
		for (const auto& c : _children) {
			if(c == child) _children.remove(child);
		}
	}

	bool SceneNode::hasParent() const {
		return _parent != nullptr;
	}

	bool SceneNode::hasChildren() const {
		return _children.size();
	}

	std::string SceneNode::name() const {
		return _name;
	}

	SceneNodeType SceneNode::type() const {
		return _type;
	}

	std::list<Shared<SceneNode>>& SceneNode::children() {
		return _children;
	}

	Shared<SceneNode> SceneNode::parent() {
		return _parent;
	}







	TransformNode::TransformNode(const std::string& name)
		: SceneNode(name, SceneNodeType::TRANSFORM),
		position_(glm::vec3(0.0f)),
		rotation_(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
		scale_(glm::vec3(1.0f)) {}

	void TransformNode::SetPosition(const glm::vec3& position) {
		position_ = position;
	}

	void TransformNode::SetRotation(const glm::quat& rotation) {
		rotation_ = rotation;
	}

	void TransformNode::SetScale(const glm::vec3& scale) {
		scale_ = scale;
	}

	glm::vec3 TransformNode::GetPosition() const {
		return position_;
	}

	glm::quat TransformNode::GetRotation() const {
		return rotation_;
	}

	glm::vec3 TransformNode::GetScale() const {
		return scale_;
	}

	glm::mat4 TransformNode::GetModelMatrix() const {
		glm::mat4 model(1.0f);
		model = glm::translate(model, position_);
		model *= glm::mat4_cast(rotation_);
		model = glm::scale(model, scale_);
		return model;
	}

	void TransformNode::Draw(const Camera& camera) const {
		// TransformNode itself doesn't have any drawable components.
		// Just propagate the draw call to its children.
		for (const auto& child : _children) {
			child->Draw(camera);
		}
	}



}