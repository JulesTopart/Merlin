#include "glpch.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Merlin {

	Camera::Camera() :
		_width(1080),
		_height(720),
		_AspectRatio(float(_width) / float(_height)),
		_projection(Projection::Perspective),
		_fov(45.0f),
		_nearPlane(0.1f),
		_farPlane(2000.f),
		_zoom(1.0f)
	{
		Reset();
		ResetProjection();
		_ViewMatrix = glm::mat4(1.0f);
		_ViewProjectionMatrix = _ProjectionMatrix * _ViewMatrix;
		RecalculateViewMatrix();
	}

	Camera::Camera(float width, float height, Projection projection) :
		_width(width), 
		_height(height), 
		_AspectRatio(float(width)/float(height)), 
		_projection(projection),
		_fov(45.0f),
		_nearPlane(0.1f),
		_farPlane(2000.f), 
		_zoom(1.0f)
	{
		Reset();
		ResetProjection();
		_ViewMatrix = glm::mat4(1.0f);
		_ViewProjectionMatrix = _ProjectionMatrix * _ViewMatrix;
		RecalculateViewMatrix();
	}

	void Camera::OnEvent(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizeEvent>(MERLIN_BIND_EVENT_FN(Camera::OnWindowResized));
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

	void Camera::ResetProjection(){
		_AspectRatio = (float)_width / (float)_height;
		if (_projection == Projection::Perspective)
			_ProjectionMatrix = glm::perspective(glm::radians(_fov), _AspectRatio, _nearPlane, _farPlane);
		else
			_ProjectionMatrix = glm::ortho(-_AspectRatio *_zoom, _AspectRatio * _zoom, -_zoom, _zoom, _nearPlane, _farPlane);
	}

	void Camera::Translate(float dx, float dy) {
		Translate(glm::vec2(dx, dy));
	}

	void Camera::Translate(glm::vec2 dU2D) {
		Translate(glm::vec3(dU2D.y, dU2D.x, 0));
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

	bool Camera::OnWindowResized(WindowResizeEvent& e) {
		if (e.GetHeight() == 0 || e.GetWidth() == 0) return false;
		_height = e.GetHeight();
		_width = e.GetWidth();
		
		ResetProjection();

		return false;
	}

	void Camera::RecalculateViewMatrix() {

		glm::vec3 direction = { 1.0f, 0.0f, 0.0f };
		glm::vec3 eulerAngles = _Rotation;
		glm::fquat quaternion{ glm::radians(eulerAngles) };
		
		direction = quaternion * direction;

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