#include "glpch.h"
#include "CameraController.h"
#include "Merlin/Core/Core.h"
#include "Merlin/Core/Input.h"
#include "Merlin/Core/KeyCodes.h"

namespace Merlin::Graphics {

	void CameraController::SetZoomLevel(float zl)
	{
		_ZoomLevel = zl;
		_Camera->setZoom(zl);
	}

	CameraController3D::CameraController3D(Shared<Camera> cam) : CameraController(cam) {}

	void CameraController3D::OnUpdate(Timestep ts)
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

		_dR *= ts * 2.0;
		_Camera->Translate(_dU);

		if (_Camera->GetRotation().y + _dR.y > -90 && _Camera->GetRotation().y + _dR.y < 90)
			_Camera->Rotate(_dR);

		_dU = glm::vec3(0.0f);
		_dR = glm::vec3(0.0f);
	}

	void CameraController3D::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(MERLIN_BIND_EVENT_FN(CameraController3D::OnMouseScrolled));
		dispatcher.Dispatch<MouseMovedEvent>(MERLIN_BIND_EVENT_FN(CameraController3D::OnMouseMoved));
	}


	bool CameraController3D::OnMouseMoved(MouseMovedEvent& e) {
		glm::vec2 newMousePos = glm::vec2(e.GetX(), e.GetY());
		

		if (Input::IsMouseButtonPressed(MRL_MOUSE_BUTTON_RIGHT)) { //Mouse dragged
			_deltaMousePos = _lastMousePos - newMousePos;

			_dR.z = -_deltaMousePos.x;
			_dR.y = _deltaMousePos.y;
		}

		_lastMousePos = newMousePos;
		return false;
	}

	bool CameraController3D::OnMouseScrolled(MouseScrolledEvent& e)
	{
		_CameraSpeed += e.GetYOffset() * 0.25f * _CameraSpeed;
		_CameraSpeed = std::max<float>(_CameraSpeed, 0.25f);
		return false;
	}


	CameraController2D::CameraController2D(Shared<Camera> cam) : CameraController(cam){}

	void CameraController2D::OnUpdate(Timestep ts)
	{
		if (Input::IsKeyPressed(MRL_KEY_A))
			_dU.x -= _CameraSpeed * ts;
		else if (Input::IsKeyPressed(MRL_KEY_D))
			_dU.x += _CameraSpeed * ts;
		if (Input::IsKeyPressed(MRL_KEY_W))
			_dU.y -= _CameraSpeed * ts;
		else if (Input::IsKeyPressed(MRL_KEY_S))
			_dU.y += _CameraSpeed * ts;

		_Camera->Translate(_dU);

		_dU = glm::vec3(0.0f);
	}

	void CameraController2D::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(MERLIN_BIND_EVENT_FN(CameraController2D::OnMouseScrolled));
		dispatcher.Dispatch<MouseMovedEvent>(MERLIN_BIND_EVENT_FN(CameraController2D::OnMouseMoved));
	}
	





	bool CameraController2D::OnMouseScrolled(MouseScrolledEvent& e)
	{
		_ZoomLevel -= e.GetYOffset() * 0.25f * _ZoomLevel;
		_ZoomLevel = std::min<float>(_ZoomLevel, 100.f);
		_ZoomLevel = std::max<float>(_ZoomLevel, 0.1);
		_Camera->setZoom(_ZoomLevel);
		return false;
	}

	bool CameraController2D::OnMouseMoved(MouseMovedEvent& e) {
		glm::vec2 newMousePos = glm::vec2(e.GetX(), e.GetY());

		if (Input::IsMouseButtonPressed(MRL_MOUSE_BUTTON_RIGHT)) { //Mouse dragged
			_deltaMousePos = _lastMousePos - newMousePos;

			_dU.x = _CameraSpeed * _deltaMousePos.x / _Camera->Width();
			_dU.y = _CameraSpeed * _deltaMousePos.y / _Camera->Height();
		}

		_lastMousePos = newMousePos;
		return false;
	}
}