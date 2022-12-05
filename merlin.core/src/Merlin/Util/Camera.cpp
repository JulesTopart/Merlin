#include "glpch.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Merlin::Utils {

	Camera::Camera()
		: _ProjectionMatrix(glm::perspective(45.0f, 16.0f / 9.0f, 0.01f, 100.f)), _ViewMatrix(1.0f)
	{
		_ViewProjectionMatrix = _ProjectionMatrix * _ViewMatrix;
		RecalculateViewMatrix();
	}

	void Camera::Reset(){
		_Position = { 0.0f, 0.0f, 0.0f };
		_Rotation = { 0.0f, 0.0f, 0.0f };

		_Target = { 0.0f, 0.0f, 0.0f };
		_WorldUp = { 0.0f, 0.0f, 1.0f };

		_Right = { 1.0f, 0.0f, 0.0f };
		_Front = { 0.0f, 1.0f, 0.0f };
		_Up = { 0.0f, 0.0f, 1.0f };
	}

	void Camera::Translate(float dx, float dy, float dz) {
		Translate(glm::vec3(dx, dy, dz));
	}

	void Camera::Translate(glm::vec3 du) {
		glm::vec3 transform(0.0f, 0.0f, 0.0f);

		transform += _Right * du.x;
		transform += _Front * du.y;
		transform += _Up * du.z;

		_Position += transform;
		_Target += transform;

		RecalculateViewMatrix();
	}

	void Camera::Rotate(float dRx, float dRy, float dRz) {
		Rotate(glm::vec3(dRx, dRy, dRz));
	}

	void Camera::Rotate(glm::vec3 dR) {
		_Rotation += dR;

		RecalculateViewMatrix();
	}


	void Camera::SetOrthographic(float aspectRatio, float nearPlane, float farPlane) {
		_OrthoGraphic = true;
		_ProjectionMatrix = glm::ortho(0.0f, aspectRatio, 0.0f, aspectRatio, nearPlane, farPlane);
		_ViewProjectionMatrix = _ProjectionMatrix * _ViewMatrix;
	}

	void Camera::SetPerspective(float fov, float aspectRatio, float nearPlane, float farPlane) {
		_OrthoGraphic = false;
		_ProjectionMatrix = glm::perspective(fov, aspectRatio, nearPlane, farPlane);
		_ViewProjectionMatrix = _ProjectionMatrix * _ViewMatrix;
	}

	void Camera::RecalculateViewMatrix() {

		glm::vec3 direction;
		
		direction.x = cos(glm::radians(_Rotation.y)) * cos(glm::radians(_Rotation.x));
		direction.y = sin(glm::radians(_Rotation.x));
		direction.z = sin(glm::radians(_Rotation.y)) * cos(glm::radians(_Rotation.x));
		
		

		_Front = normalize(direction);
		_Right = glm::normalize(glm::cross(_Front, _WorldUp));
		_Up = glm::normalize(glm::cross(_Right, _Front));

		if (isOrthoGraphic()) {
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), _Position);
			_ViewMatrix = glm::inverse(transform);
		}else{
			_ViewMatrix = glm::lookAt(_Position, _Position + _Front, _Up);
		}
		_ViewProjectionMatrix = _ProjectionMatrix * _ViewMatrix;
	}

}