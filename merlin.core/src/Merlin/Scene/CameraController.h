#pragma once
#include "Merlin/Core/Core.h"

#include "Camera.h"
#include "Merlin/Core/Timestep.h"

#include "Merlin/Events/ApplicationEvent.h"
#include "Merlin/Events/MouseEvent.h"

namespace Merlin::Graphics {

	class CameraController {
	public:

		CameraController(Shared<Camera> cam) : _Camera(cam) {}
		virtual void OnUpdate(Timestep ts) = 0;
		virtual void OnEvent(Event& e) = 0;

		Camera& GetCamera() const { return *_Camera; }
		Shared<Camera> ShareCamera() const { return _Camera; }
		float GetCameraSpeed() const { return _CameraSpeed; }
		void SetCameraSpeed(float speed) { _CameraSpeed = speed; }
		void SetZoomLevel(float zl);


	protected:
		Shared<Camera> _Camera;
		float _CameraSpeed = 1.0f;
		float _ZoomLevel = 1.0f;
	};

	class CameraController3D : public CameraController {
	public:
		CameraController3D(Shared<Camera> cam);
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
		void OnUpdate(Timestep ts) override;
		void OnEvent(Event& e) override;
		
	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnMouseMoved(MouseMovedEvent& e);
		glm::vec2 _dU = { 0.0f, 0.0f };
		glm::vec2 _lastMousePos = { 0.0f, 0.0f };
		glm::vec2 _deltaMousePos = { 0.0f, 0.0f };
	};

	typedef Shared<CameraController> CameraController_Ptr;
}