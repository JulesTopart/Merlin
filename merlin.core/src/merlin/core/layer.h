#pragma once
#include "merlin/core/core.h"
#include "timestep.h"
#include "../events/event.h"
#include "merlin/utils/openGLDebug.h"
#include "merlin/scene/camera.h"
#include "merlin/scene/cameraController.h"

namespace Merlin {

	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() = default;

		virtual void onAttach() {}
		virtual void onDetach() {}
		virtual void onUpdate(Timestep ts) { updateFPS(ts); }
		virtual void onImGuiRender() {}
		virtual void onEvent(Event& event) {}

		int fps();

		inline const std::string& getName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	private:
		void updateFPS(Timestep ts);
		int m_FPS = 0;
		float m_accFrametime = 0;
		float m_FrameCount = 0;

	};

	class Layer2D : public Layer
	{
	public:
		Layer2D(const std::string& name = "Layer");
		virtual ~Layer2D() = default;

		virtual void onAttach() override { enableGLDebugging(); }
		virtual void onDetach() override {}
		virtual void onUpdate(Timestep ts) override { cameraController->onUpdate(ts); Layer::onUpdate(ts); }
		virtual void onImGuiRender() override {}
		virtual void onEvent(Event& event) override {
			m_camera->onEvent(event);
			cameraController->onEvent(event);
		}

		inline Camera& camera() { return *m_camera; };
		inline Shared<Camera> getCamera() { return m_camera; };
	private:
		Shared<Camera> m_camera;
		Shared<CameraController2D> cameraController;
	};

	class Layer3D : public Layer
	{
	public:
		Layer3D(const std::string& name = "Layer");
		virtual ~Layer3D() = default;

		virtual void onAttach() override { enableGLDebugging();}
		virtual void onDetach() override {}
		virtual void onUpdate(Timestep ts) override { cameraController->onUpdate(ts); Layer::onUpdate(ts); }
		virtual void onImGuiRender() override {}
		virtual void onEvent(Event& event) override {
			m_camera->onEvent(event);
			cameraController->onEvent(event);
		}

		inline Camera& camera() { return *m_camera; };
		inline Shared<Camera> getCamera() { return m_camera; };

	private:
		Shared<Camera> m_camera;
		Shared<CameraController3D> cameraController;
	};

}