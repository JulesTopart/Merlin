#include "glpch.h"
#include "Merlin/Core/Core.h"
#include "Merlin/Graphics/RenderableObject.h"
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Merlin::Graphics {
	
	int RenderableObject::nextID = 0;

	std::string RenderableObject::TypeToString(ObjectType ob) {

		switch (ob){
		case Merlin::Graphics::ObjectType::SCENE:
			return "Scene";
			break;
		case Merlin::Graphics::ObjectType::MODEL:
			return "Model";
			break;
		case Merlin::Graphics::ObjectType::MESH:
			return "Mesh";
			break;
		case Merlin::Graphics::ObjectType::SKYBOX:
			return "SkyBox";
			break;
		case Merlin::Graphics::ObjectType::TRANSFORM:
			return "Transform";
			break;
		case Merlin::Graphics::ObjectType::PARTICLESYSTEM:
			return "ParticleSystem";
			break;
		default:
			return "GenericObject";
			break;
		}
	}

	RenderableObject::RenderableObject() : m_type(ObjectType::GENERIC), m_parent(nullptr), m_transform(glm::mat4(1)) {
		m_ID = nextID++;
		m_name = TypeToString(m_type) + std::to_string(m_ID);
	}

	RenderableObject::RenderableObject(std::string name, ObjectType type) : m_type(type), m_parent(nullptr), m_name(name), m_transform(glm::mat4(1)) {
		m_ID = nextID++;
	}

	Shared<RenderableObject> RenderableObject::GetChild(std::string name) {
		for (auto child : m_children) {
			if (child->name() == name) return child;
		}
		return nullptr;
	}

	void RenderableObject::AddChild(const Shared<RenderableObject>& child) {
		if (child != nullptr) {
			m_children.push_back(child);
			child->SetParent(this);
		}
		else Console::warn("SceneNode") << "Empty node child ignored" << Console::endl;
	}
	void RenderableObject::RemoveChild(Shared<RenderableObject> child) {
		m_children.remove(child);
	}

	void RenderableObject::SetParent(RenderableObject* parent) {
		m_parent = parent;
	}

	bool RenderableObject::HasParent() const {
		return m_parent != nullptr;
	}

	bool RenderableObject::HasChildren() const {
		return m_children.size();
	}


	std::list<Shared<RenderableObject>>& RenderableObject::children() {
		return m_children;
	}

	RenderableObject* RenderableObject::parent() {
		return m_parent;
	}


	void RenderableObject::Translate(glm::vec3 v) {
		m_transform = glm::translate(m_transform, v);
	}

	void RenderableObject::Scale(glm::vec3 v) {
		m_transform = glm::scale(m_transform, v);
	}

	void RenderableObject::Scale(float v) {
		m_transform = glm::scale(m_transform, glm::vec3(v));
	}

	void RenderableObject::SetPosition(glm::vec3 v) {
		m_transform = glm::translate(m_transform, v - position());
	}

	void RenderableObject::Rotate(glm::vec3 v) {
		m_transform = glm::rotate(m_transform, v.x, glm::vec3(1, 0, 0));
		m_transform = glm::rotate(m_transform, v.y, glm::vec3(0, 1, 0));
		m_transform = glm::rotate(m_transform, v.z, glm::vec3(0, 0, 1));
	}

	void RenderableObject::Rotate(float angle, glm::vec3 v) {
		m_transform = glm::rotate(m_transform, angle, v);
	}

	void RenderableObject::SetTransform(glm::mat4 t) {
		m_transform = t;
	}

	const glm::vec3& RenderableObject::position() const {
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(m_transform, scale, rotation, translation, skew, perspective);
		return translation;
	}

	const glm::quat& RenderableObject::rotation() const {
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(m_transform, scale, rotation, translation, skew, perspective);
		return rotation;
	}

	


}