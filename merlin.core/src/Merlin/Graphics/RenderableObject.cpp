#include "glpch.h"
#include "Merlin/Core/Core.h"
#include "Merlin/Graphics/RenderableObject.h"
#include <glm/gtx/matrix_decompose.hpp>

namespace Merlin::Graphics {

	void RenderableObject::Translate(glm::vec3 v) {
		_transform = glm::translate(_transform, v);
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