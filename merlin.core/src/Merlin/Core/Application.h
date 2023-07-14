#pragma once
#include "Merlin/Core/Core.h"


#include "Window.h"
#include "LayerStack.h"
#include "../Events/Event.h"
#include "../Events/ApplicationEvent.h"

#include "Timestep.h"

#include "../UI/ImGuiLayer.h"

namespace Merlin {

	class Application
	{
	public:
		Application(const std::string& name = "OpenGL Sandbox", uint32_t width = 1280, uint32_t height = 720, bool vsync = true, bool multisampling = true, bool fullscreen = false);
		virtual ~Application() = default;

		void Run();
		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void PrintHeader() const;

		void ToggleVSync();
		void ToggleFullscreen();
		void DisableMSAA();
		void ToggleMSAA();
		void EnableMSAA(MSAA_PRESET preset);

		inline Window& GetWindow() { return *m_Window; }
		inline static Application& Get() { return *s_Instance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
		void InitWindow(const std::string& name, uint32_t width, uint32_t height, bool vsync, bool multisampling, bool fullscreen);

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;

		static Application* s_Instance;
	};

}