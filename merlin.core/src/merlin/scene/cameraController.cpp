#include "glpch.h"
#include "cameraController.h"
#include "merlin/core/core.h"
#include "merlin/core/input.h"
#include "merlin/core/keyCodes.h"

namespace Merlin {

	void CameraController::setZoomLevel(float zl)
	{
		_ZoomLevel = zl;
		_Camera->setZoom(zl);
	}

	CameraController3D::CameraController3D(Shared<Camera> cam) : CameraController(cam) {}

	void CameraController3D::onUpdate(Timestep ts)
	{
		if (Input::isKeyPressed(MRL_KEY_A))
		{
			_dU.x -= _CameraSpeed *ts;
		}
		else if (Input::isKeyPressed(MRL_KEY_D))
		{
			_dU.x += _CameraSpeed *ts;
		}

		if (Input::isKeyPressed(MRL_KEY_W))
		{
			_dU.y += _CameraSpeed *ts;
		}
		else if (Input::isKeyPressed(MRL_KEY_S))
		{
			_dU.y -= _CameraSpeed *ts;
		}

		if (Input::isKeyPressed(MRL_KEY_LEFT_CONTROL))
		{
			_dU.z -= _CameraSpeed * ts;
		}
		else if (Input::isKeyPressed(MRL_KEY_SPACE))
		{
			_dU.z += _CameraSpeed * ts;
		}

		
		if (Input::isKeyPressed(MRL_KEY_Q))
			_dR.x += _CameraSpeed;
		if (Input::isKeyPressed(MRL_KEY_E))
			_dR.x -= _CameraSpeed;

		_dR *= ts * 2.0;
		_Camera->translate(_dU);

		if (_Camera->getRotation().y + _dR.y > -90 && _Camera->getRotation().y + _dR.y < 90)
			_Camera->rotate(_dR);

		_dU = glm::vec3(0.0f);
		_dR = glm::vec3(0.0f);
	}

	void CameraController3D::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseScrolledEvent>(MERLIN_BIND_EVENT_FN(CameraController3D::onMouseScrolled));
		dispatcher.dispatch<MouseMovedEvent>(MERLIN_BIND_EVENT_FN(CameraController3D::onMouseMoved));
	}


	bool CameraController3D::onMouseMoved(MouseMovedEvent& e) {
		glm::vec2 newMousePos = glm::vec2(e.getX(), e.getY());
		

		if (Input::isMouseButtonPressed(MRL_MOUSE_BUTTON_RIGHT)) { //Mouse dragged
			_deltaMousePos = _lastMousePos - newMousePos;

			_dR.z = -_deltaMousePos.x * std::min(_CameraSpeed * 0.5f, 50.0f);
			_dR.y = _deltaMousePos.y * std::min(_CameraSpeed * 0.5f, 50.0f);
		}

		_lastMousePos = newMousePos;
		return false;
	}

	bool CameraController3D::onMouseScrolled(MouseScrolledEvent& e)
	{
		_CameraSpeed += e.getYOffset() * 0.25f * _CameraSpeed;
		_CameraSpeed = std::max<float>(_CameraSpeed, 0.25f);
		return false;
	}


	CameraController2D::CameraController2D(Shared<Camera> cam) : CameraController(cam){}

	void CameraController2D::onUpdate(Timestep ts)
	{
		if (Input::isKeyPressed(MRL_KEY_A))
			_dU.x -= _ZoomLevel * _CameraSpeed * ts;
		else if (Input::isKeyPressed(MRL_KEY_D))
			_dU.x += _ZoomLevel * _CameraSpeed * ts;
		if (Input::isKeyPressed(MRL_KEY_W))
			_dU.y -= _ZoomLevel * _CameraSpeed * ts;
		else if (Input::isKeyPressed(MRL_KEY_S))
			_dU.y += _ZoomLevel * _CameraSpeed * ts;

		_Camera->translate(_dU);

		_dU = glm::vec3(0.0f);
	}

	void CameraController2D::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseScrolledEvent>(MERLIN_BIND_EVENT_FN(CameraController2D::onMouseScrolled));
		dispatcher.dispatch<MouseMovedEvent>(MERLIN_BIND_EVENT_FN(CameraController2D::onMouseMoved));
	}
	





	bool CameraController2D::onMouseScrolled(MouseScrolledEvent& e)
	{
		_ZoomLevel -= e.getYOffset() * 0.25f * _ZoomLevel;
		_ZoomLevel = std::min<float>(_ZoomLevel, 500.f);
		_ZoomLevel = std::max<float>(_ZoomLevel, 0.1);
		_Camera->setZoom(_ZoomLevel);
		Console::print() << _ZoomLevel << Console::endl;
		return false;
	}

	bool CameraController2D::onMouseMoved(MouseMovedEvent& e) {
		glm::vec2 newMousePos = glm::vec2(e.getX(), e.getY());

		if (Input::isMouseButtonPressed(MRL_MOUSE_BUTTON_RIGHT)) { //Mouse dragged
			_deltaMousePos = _lastMousePos - newMousePos;

			_dU.x = 3.0 * _ZoomLevel * _CameraSpeed * _deltaMousePos.x / _Camera->width();
			_dU.y = 3.0 * _ZoomLevel * _CameraSpeed * _deltaMousePos.y / _Camera->height();
		}

		_lastMousePos = newMousePos;
		return false;
	}
}