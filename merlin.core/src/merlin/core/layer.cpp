#include "glpch.h"
#include "layer.h"
#include "merlin/core/application.h"
#include "merlin/core/window.h"

namespace Merlin {

	Layer::Layer(const std::string& debugName)
		: m_DebugName(debugName){
	}

	void Layer::updateFPS(Timestep ts) {
		if (m_FrameCount == 0) {
			m_accFrametime = ts;
		}
		else {
			m_accFrametime += ts;
		}
		m_FrameCount++;

		if (m_FrameCount > 0) {
			m_FPS = 1.0f / (m_accFrametime / m_FrameCount);
		}
		if(m_FrameCount > 5) m_FrameCount = 0;
	}

	int Layer::fps() {
		return m_FPS;
	}

	Layer2D::Layer2D(const std::string& debugName) : Layer(debugName){
		Window* w = &Application::get().getWindow();
		int height = w->getHeight();
		int width = w->getWidth();
		m_camera = createShared<Camera>(width, height, Projection::Orthographic);
		m_camera->translate(glm::vec3(0, 0, 1));
		cameraController = createShared<CameraController2D>(m_camera);
		cameraController->setCameraSpeed(1);
	}

	Layer3D::Layer3D(const std::string& debugName) : Layer(debugName){
		Window* w = &Application::get().getWindow();
		int height = w->getHeight();
		int width = w->getWidth();
		m_camera = createShared<Camera>(width, height, Projection::Perspective);
		cameraController = createShared<CameraController3D>(m_camera);
		cameraController->setZoomLevel(1);
		//cameraController->setCameraSpeed(100);
	}
	

}