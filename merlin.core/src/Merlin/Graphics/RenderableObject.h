#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Scene/Camera.h"
#include "Merlin/Memory/VertexArray.h"

namespace Merlin {

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

		ObjectType getType() { return m_type; }
		static std::string typeToString(ObjectType);

		virtual void draw(){};

		//Transformation
		void translate(glm::vec3);
		void rotate(glm::vec3);
		void rotate(float angle, glm::vec3 v);
		void setTransform(glm::mat4 t);
		void setPosition(glm::vec3 v);
		void scale(glm::vec3 v);
		void scale(float v);

		//Getters
		const glm::vec3& position() const;
		const glm::quat& rotation() const;
		inline const glm::mat4& transform() const { return m_transform; }
		inline const std::string name() const { return m_name; }
		inline void rename(std::string n) { m_name = n; };

		//Hierachy
		void addChild(const Shared<RenderableObject>& child);
		void removeChild(Shared<RenderableObject> child);
		void setParent(RenderableObject* parent);

		inline void show() { m_hidden = false; }
		inline void hide() { m_hidden = true; }
		inline bool isHidden() { return m_hidden; }

		bool hasParent() const;
		bool hasChildren() const;

		inline bool isWireFrame() const { return m_wireframe; }
		inline void enableWireFrameMode() { m_wireframe = true; }
		inline void disableWireFrameMode() { m_wireframe = false; }

		std::list<Shared<RenderableObject>>& children();
		Shared<RenderableObject> getChild(std::string name);
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
