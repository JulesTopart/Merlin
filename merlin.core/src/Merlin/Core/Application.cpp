#include "glpch.h"
#include "Application.h"

#include "Log.h"
#include "Input.h"

#include <glfw/glfw3.h>


namespace Merlin {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& name, uint32_t width, uint32_t height, bool vsync, bool multisampling, bool fullscreen){
		if (!s_Instance)
		{
			// Initialize core
			//LOG_INFO("Application") << "Initializing core" << Console::endl;
		}
 
		GLCORE_ASSERT(!s_Instance, "Application", "Application already exists!");
		s_Instance = this;


		initWindow(name, width, height, vsync, multisampling, fullscreen);
		printHeader();

		m_ImGuiLayer = new ImGuiLayer();
		pushOverlay(m_ImGuiLayer);
	}

	void Application::initWindow(const std::string& name, uint32_t width, uint32_t height, bool vsync, bool multisampling, bool fullscreen) {
		m_Window = std::unique_ptr<Window>(Window::create({ name, width, height, vsync, multisampling, fullscreen }));
		m_Window->setEventCallback(BIND_EVENT_FN(onEvent));
	}

	void Application::toggleVSync() {

	}

	void Application::toggleFullscreen() {

	}

	void Application::disableMSAA() {

	}

	void Application::toggleMSAA() {

	}

	void Application::enableMSAA(MSAA_PRESET preset) {

	}



	void Application::printHeader() const {
		//int deviceMem;
		//glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &deviceMem);

		Console::print()
			<< "| -------------- | " << "------------------------------------------------------------" << "|" << Console::endl
			<< "| Merlin Engine  | " << Console::alignLeft(60, "v1.0.8") << "|" << Console::endl
			<< "| -------------- | " << "------------------------------------------------------------" << "|" << Console::endl
			<< "| GPU vendor     | " << Console::alignLeft(60, (const char*)glGetString(GL_VENDOR)) << "|" << Console::endl
			<< "| GPU name       | " << Console::alignLeft(60, (const char*)glGetString(GL_RENDERER)) << "|" << Console::endl
			<< "| OpenGL version | " << Console::alignLeft(60, (const char*)glGetString(GL_VERSION)) << "|" << Console::endl
			<< "| GLSL version   | " << Console::alignLeft(60, (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION)) << "|" << Console::endl
			//<< "| GPU RAM        | " << Console::alignLeft(60, std::to_string(deviceMem / 1000000.0f) + " Go") << "|" << Console::endl
			<< "| -------------- | " << "------------------------------------------------------------" << "|" << Console::endl;
	}

	void Application::pushLayer(Layer* layer)
	{
		m_LayerStack.pushLayer(layer);
	}

	void Application::pushOverlay(Layer* layer)
	{
		m_LayerStack.pushOverlay(layer);
	}

	void Application::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(onWindowClose));
		dispatcher.dispatch<WindowResizeEvent>(BIND_EVENT_FN(onWindowResized));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->onEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::run()
	{
		while (m_Running)
		{
			double time = (double)glfwGetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			for (Layer* layer : m_LayerStack)
				layer->onUpdate(timestep);

			m_ImGuiLayer->begin();
			for (Layer* layer : m_LayerStack)
				layer->onImGuiRender();
			m_ImGuiLayer->end();

			m_Window->onUpdate();

		}
	}

	bool Application::onWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}	

	bool Application::onWindowResized(WindowResizeEvent& e)
	{
		if (e.getHeight() == 0 || e.getWidth() == 0) return false;
		glViewport(0, 0, e.getWidth(), e.getHeight());
		return false;
	}

}