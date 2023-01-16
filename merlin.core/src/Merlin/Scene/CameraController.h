#pragma once

#include "Camera.h"
#include "Merlin/Core/Timestep.h"

#include "Merlin/Events/ApplicationEvent.h"
#include "Merlin/Events/MouseEvent.h"

namespace Merlin::Scene {

	class CameraController
	{
	public:
		CameraController(float fov, float aspectRatio, float nearPlane = 0.01f, float farPlane = 1000.0f);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		Camera& GetCamera() { return _Camera; }
		const Camera& GetCamera() const { return _Camera; }

		float GetCameraSpeed() const { return _CameraSpeed; }
		void SetCameraSpeed(float speed) { _CameraSpeed = speed; }
	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnMouseMoved(MouseMovedEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		float _AspectRatio,
			 _fov,
			 _CameraSpeed,
			 _farPlane,
			 _nearPlane;
		
		Camera _Camera;

		glm::vec2 _lastMousePos = { 0.0f, 0.0f};
		glm::vec2 _deltaMousePos = { 0.0f, 0.0f };

		glm::vec3 _dU = { 0.0f, 0.0f, 0.0f };
		glm::vec3 _dR = { 0.0f, 0.0f, 0.0f }; //In degrees, in the anti-clockwise direction, yaw, pitch, roll
	};

}