#include "glpch.h"
#include "SceneNode.h"

namespace Merlin::Graphics {

	int SceneNode::nextID = 0;

	SceneNode::SceneNode(const std::string& name) : name_(name)  {
		ID = nextID++;
	}

	void SceneNode::AddChild(const Shared<SceneNode>& child) {
		children_.push_back(child);
	}

	void SceneNode::SetObject(const Shared<SceneObject>& object) {
		object_ = object;
	}

	void SceneNode::RemoveChild(const Shared<SceneNode>& child) {
		for (auto it = children_.begin(); it != children_.end(); ++it) {
			if (*it == child) {
				children_.erase(it);
				break;
			}
		}
	}

	void SceneNode::Clear() {
		children_.clear();
	}

	const std::string& SceneNode::name() const {
		return name_;
	}

	bool SceneNode::hasObject() const {
		return object_ != nullptr;
	}

	bool SceneNode::hasParent() const {
		return parent_ != nullptr;
	}

	bool SceneNode::hasChildren() const {
		return children_.size() > 0;
	}

	const Shared<SceneObject>& SceneNode::object() const {
		return object_;
	}

	const std::vector<Shared<SceneNode>>& SceneNode::children() const {
		return children_;
	}

	Shared<SceneNode> SceneNode::parent() const {
		return parent_;
	}




}