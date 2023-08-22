#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Scene/Camera.h"
#include "Merlin/Memory/VertexArray.h"

using namespace Merlin::Memory;

namespace Merlin::Graphics {

	enum class ObjectTypes {
		GENERIC,
		SCENE,
		MODEL,
		MESH, 
		SKYBOX,
		TRANSFORM, 
	};

	class RenderableObject {
	public:
		RenderableObject(std::string name, RenderableObject* parent = nullptr);

		virtual ObjectTypes GetType() { return ObjectTypes::GENERIC; }

		//Transformation
		void Translate(glm::vec3);
		void Rotate(glm::vec3);
		void Rotate(float angle, glm::vec3 v);
		void SetTransform(glm::mat4 t);
		void SetPosition(glm::vec3 v);

		//Getters
		const glm::vec3& position() const;
		const glm::quat& rotation() const;
		inline const glm::mat4& transform() const { return _transform; }
		inline const std::string name() const { return _name; }
		inline void SetName(std::string n) { _name = n; };

		//Hierachy
		void AddChild(const Shared<RenderableObject>& child);
		void RemoveChild(Shared<RenderableObject> child);
		void SetParent(RenderableObject* parent);

		inline void Show() { _hidden = false; }
		inline void Hide() { _hidden = true; }
		inline bool IsHidden() { return _hidden; }

		bool HasParent() const;
		bool HasChildren() const;

		inline bool IsWireFrame() const { return _wireframe; }
		inline void EnableWireFrameMode() { _wireframe = true; }
		inline void DisableWireFrameMode() { _wireframe = false; }

		std::list<Shared<RenderableObject>>& children();
		RenderableObject* parent();

	protected:
		static int nextID;

		int _ID;
		RenderableObject* _parent = nullptr;
		std::list<Shared<RenderableObject>> _children;

	protected:
		std::string _name;
		glm::mat4 _transform;
		bool _wireframe = false;
		bool _hidden = false;
	};


}
