#pragma once
#include "window.h"
#include "layerStack.h"
#include "../events/applicationevent.h"

#include "timestep.h"

#include "../ui/imguilayer.h"

namespace Merlin {

	class Application
	{
	public:
		Application(const std::string& name = "OpenGL Sandbox", uint32_t width = 1280, uint32_t height = 720, bool vsync = true, bool multisampling = true, bool fullscreen = false);
		virtual ~Application() = default;

		void run();
		void onEvent(Event& e);

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* layer);
		void printHeader() const;

		void toggleVSync();
		void toggleFullscreen();
		void disableMSAA();
		void toggleMSAA();
		void enableMSAA(MSAA_PRESET preset);

		inline Window& getWindow() { return *m_Window; }
		inline static Application& get() { return *s_Instance; }
	private:
		bool onWindowClose(WindowCloseEvent& e);
		bool onWindowResized(WindowResizeEvent& e);
		void initWindow(const std::string& name, uint32_t width, uint32_t height, bool vsync, bool multisampling, bool fullscreen);

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;

		static Application* s_Instance;
	};

}