#include "glpch.h"
#include "Merlin/Core/Core.h"
#include "Merlin/Graphics/RenderableObject.h"
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Merlin::Graphics {
	
	int RenderableObject::nextID = 0;

	void RenderableObject::AddChild(const Shared<RenderableObject>& child) {
		if (child != nullptr) {
			_children.push_back(child);
			child->SetParent(this);
		}
		else Console::warn("SceneNode") << "Empty node child ignored" << Console::endl;
	}
	void RenderableObject::RemoveChild(Shared<RenderableObject> child) {
		_children.remove(child);
	}

	void RenderableObject::SetParent(RenderableObject* parent) {
		_parent = parent;
	}

	bool RenderableObject::HasParent() const {
		return _parent != nullptr;
	}

	bool RenderableObject::HasChildren() const {
		return _children.size();
	}


	std::list<Shared<RenderableObject>>& RenderableObject::children() {
		return _children;
	}

	RenderableObject* RenderableObject::parent() {
		return _parent;
	}


	RenderableObject::RenderableObject(std::string name, RenderableObject* parent) : _parent(parent), _name(name), _transform(glm::mat4(1)) {
		_ID = nextID++;
	}

	void RenderableObject::Translate(glm::vec3 v) {
		_transform = glm::translate(_transform, v);
	}

	void RenderableObject::Scale(glm::vec3 v) {
		_transform = glm::scale(_transform, v);
	}

	void RenderableObject::Scale(float v) {
		_transform = glm::scale(_transform, glm::vec3(v));
	}

	void RenderableObject::SetPosition(glm::vec3 v) {
		_transform = glm::translate(_transform, v - position());
	}

	void RenderableObject::Rotate(glm::vec3 v) {
		_transform = glm::rotate(_transform, v.x, glm::vec3(1, 0, 0));
		_transform = glm::rotate(_transform, v.y, glm::vec3(0, 1, 0));
		_transform = glm::rotate(_transform, v.z, glm::vec3(0, 0, 1));
	}

	void RenderableObject::Rotate(float angle, glm::vec3 v) {
		_transform = glm::rotate(_transform, angle, v);
	}

	void RenderableObject::SetTransform(glm::mat4 t) {
		_transform = t;
	}

	const glm::vec3& RenderableObject::position() const {
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(_transform, scale, rotation, translation, skew, perspective);
		return translation;
	}

	const glm::quat& RenderableObject::rotation() const {
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(_transform, scale, rotation, translation, skew, perspective);
		return rotation;
	}

	


}