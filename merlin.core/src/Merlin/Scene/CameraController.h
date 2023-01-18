#pragma once
#include "Merlin/Core/Core.h"

#include "Camera.h"
#include "Merlin/Core/Timestep.h"

#include "Merlin/Events/ApplicationEvent.h"
#include "Merlin/Events/MouseEvent.h"

namespace Merlin::Scene {

	class CameraController {
	public:
		virtual void OnUpdate(Timestep ts) {};
		virtual void OnEvent(Event& e) {};

		Shared<Camera> GetCamera() const { return _Camera; }
		float GetCameraSpeed() const { return _CameraSpeed; }
		void SetCameraSpeed(float speed) { _CameraSpeed = speed; }

	protected:
		Shared<Camera> _Camera;
		float _CameraSpeed = 1.0f;
	};

	class CameraController3D : public CameraController {
	public:
		CameraController3D(Shared<Camera> Camera);
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnEvent(Event& e) override;

	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnMouseMoved(MouseMovedEvent& e);

		glm::vec2 _lastMousePos = { 0.0f, 0.0f};
		glm::vec2 _deltaMousePos = { 0.0f, 0.0f };

		glm::vec3 _dU = { 0.0f, 0.0f, 0.0f };
		glm::vec3 _dR = { 0.0f, 0.0f, 0.0f }; //In degrees, in the anti-clockwise direction, yaw, pitch, roll
	};


	class CameraController2D : public CameraController {
	public:
		CameraController2D(Shared<Camera> Camera);
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnEvent(Event& e) override;

	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnMouseMoved(MouseMovedEvent& e);
		float _ZoomLevel = 1.0f;
		glm::vec2 _dU = { 0.0f, 0.0f };
		glm::vec2 _lastMousePos = { 0.0f, 0.0f };
		glm::vec2 _deltaMousePos = { 0.0f, 0.0f };
	};
}