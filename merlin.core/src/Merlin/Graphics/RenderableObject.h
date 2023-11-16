#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Scene/Camera.h"
#include "Merlin/Memory/VertexArray.h"

using namespace Merlin::Memory;

namespace Merlin::Graphics {

	enum class ObjectType {
		GENERIC,
		SCENE,
		MODEL,
		MESH, 
		SKYBOX,
		TRANSFORM,
		PARTICLESYSTEM,
	};

	class RenderableObject {
	public:
		RenderableObject();
		RenderableObject(std::string name, ObjectType type = ObjectType::GENERIC );

		ObjectType GetType() { return m_type; }
		static std::string TypeToString(ObjectType);

		virtual void Draw(){};

		//Transformation
		void Translate(glm::vec3);
		void Rotate(glm::vec3);
		void Rotate(float angle, glm::vec3 v);
		void SetTransform(glm::mat4 t);
		void SetPosition(glm::vec3 v);
		void Scale(glm::vec3 v);
		void Scale(float v);

		//Getters
		const glm::vec3& position() const;
		const glm::quat& rotation() const;
		inline const glm::mat4& transform() const { return m_transform; }
		inline const std::string name() const { return m_name; }
		inline void Rename(std::string n) { m_name = n; };

		//Hierachy
		void AddChild(const Shared<RenderableObject>& child);
		void RemoveChild(Shared<RenderableObject> child);
		void SetParent(RenderableObject* parent);

		inline void Show() { m_hidden = false; }
		inline void Hide() { m_hidden = true; }
		inline bool IsHidden() { return m_hidden; }

		bool HasParent() const;
		bool HasChildren() const;

		inline bool IsWireFrame() const { return m_wireframe; }
		inline void EnableWireFrameMode() { m_wireframe = true; }
		inline void DisableWireFrameMode() { m_wireframe = false; }

		std::list<Shared<RenderableObject>>& children();
		Shared<RenderableObject> GetChild(std::string name);
		RenderableObject* parent();

	protected:
		static int nextID;
		int m_ID;

		bool m_wireframe = false;
		bool m_hidden = false;
		std::string m_name;
		glm::mat4 m_transform;

		RenderableObject* m_parent;
		ObjectType m_type;

		std::list<Shared<RenderableObject>> m_children;
	};


}
