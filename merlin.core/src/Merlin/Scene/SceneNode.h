#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Scene/SceneObject.h"

namespace Merlin::Scene {
	class SceneNode {
	public:
		SceneNode(const std::string& name);
		void AddChild(const Shared<SceneNode>& child);
		void SetObject(const Shared<SceneObject>& object);

		bool hasObject() const;
		bool hasParent() const;
		bool hasChildren() const;
		const std::string& name() const;
		
		const Shared<SceneObject>& object() const;
		const std::vector<Shared<SceneNode>>& children() const;
		Shared<SceneNode> parent() const;

	private:
		void SetParent(Shared<SceneNode> parent) {
			parent_ = parent;
		}

		std::string name_;
		Shared<SceneObject> object_ = nullptr;
		Shared<SceneNode> parent_ = nullptr;
		std::vector<Shared<SceneNode>> children_;

	};
}
