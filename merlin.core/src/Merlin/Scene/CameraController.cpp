#include "glpch.h"
#include "CameraController.h"

#include "Merlin/Core/Input.h"
#include "Merlin/Core/KeyCodes.h"

namespace Merlin::Scene {

	CameraController::CameraController(float fov, float aspectRatio, float nearPlane, float farPlane)
		: _AspectRatio(aspectRatio), _Camera(), _fov(fov), _CameraSpeed(1.0f), _nearPlane(nearPlane), _farPlane(farPlane)
	{
		_Camera.SetPerspective(fov,_AspectRatio, _nearPlane, _farPlane);
	}

	void CameraController::OnUpdate(Timestep ts)
	{
		if (Input::IsKeyPressed(MRL_KEY_A))
		{
			_dU.x -= _CameraSpeed *ts;
		}
		else if (Input::IsKeyPressed(MRL_KEY_D))
		{
			_dU.x += _CameraSpeed *ts;
		}

		if (Input::IsKeyPressed(MRL_KEY_W))
		{
			_dU.y += _CameraSpeed *ts;
		}
		else if (Input::IsKeyPressed(MRL_KEY_S))
		{
			_dU.y -= _CameraSpeed *ts;
		}

		if (Input::IsKeyPressed(MRL_KEY_LEFT_CONTROL))
		{
			_dU.z -= _CameraSpeed * ts;
		}
		else if (Input::IsKeyPressed(MRL_KEY_SPACE))
		{
			_dU.z += _CameraSpeed * ts;
		}

		
		if (Input::IsKeyPressed(MRL_KEY_Q))
			_dR.x += _CameraSpeed;
		if (Input::IsKeyPressed(MRL_KEY_E))
			_dR.x -= _CameraSpeed;

		_dR *= ts * _CameraSpeed;
		_Camera.Translate(_dU);
		_Camera.Rotate(_dR);

		_dU = glm::vec3(0.0f);
		_dR = glm::vec3(0.0f);
	}

	void CameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(GLCORE_BIND_EVENT_FN(CameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(GLCORE_BIND_EVENT_FN(CameraController::OnWindowResized));
		dispatcher.Dispatch<MouseMovedEvent>(GLCORE_BIND_EVENT_FN(CameraController::OnMouseMoved));
	}


	bool CameraController::OnMouseMoved(MouseMovedEvent& e) {
		glm::vec2 newMousePos = glm::vec2(e.GetX(), e.GetY());
		

		if (Input::IsMouseButtonPressed(MRL_MOUSE_BUTTON_RIGHT)) { //Mouse dragged
			_deltaMousePos = _lastMousePos - newMousePos;

			_dR.z = -_deltaMousePos.x;
			_dR.y = _deltaMousePos.y;
		}

		_lastMousePos = newMousePos;
		return false;
	}

	bool CameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		_CameraSpeed += e.GetYOffset() * 0.25f * _CameraSpeed;
		_CameraSpeed = std::max<float>(_CameraSpeed, 0.25f);
		return false;
	}

	bool CameraController::OnWindowResized(WindowResizeEvent& e){
		if(e.GetHeight() == 0 || e.GetWidth() == 0) return false;
		_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		_Camera.SetPerspective(_fov, _AspectRatio, _nearPlane, _farPlane);
		return false;
	}

}