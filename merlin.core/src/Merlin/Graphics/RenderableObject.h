#pragma once
#include "Merlin/Core/Core.h"

#include "Merlin/Memory/VertexArray.h"

using namespace Merlin::Memory;

namespace Merlin::Graphics {
	class RenderableObject {
	public:

		//Draw();
		virtual void Draw();
		virtual void DrawInstanced(GLsizeiptr instanced);

		//Transformation
		void Translate(glm::vec3);
		void Rotate(glm::vec3);
		void Rotate(float angle, glm::vec3 v);
		void SetTransform(glm::mat4 t);

		//Getters
		const glm::vec3& position() const;
		const glm::quat& rotation() const;
		const glm::mat4& transform() const;
		inline const glm::mat4& transform() const { return _transform; }
		inline const std::string name() const { return _name; }

	protected:
		std::string _name;
		glm::mat4 _transform;
	};


}
