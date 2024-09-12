#pragma once
#include "merlin/core/core.h"
#include "merlin/core/window.h"

#include <GLFW/glfw3.h>

namespace Merlin {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void onUpdate() override;

		inline uint32_t getWidth() const override { return m_Data.width; }
		inline uint32_t getHeight() const override { return m_Data.height; }

		// Window attributes
		inline void setEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void setVSync(bool enabled) override;
		bool isVSync() const override;

		inline virtual void* getNativeWindow() const { return m_Window; }
	private:
		virtual void init(const WindowProps& props);
		virtual void shutdown();
	private:
		GLFWwindow* m_Window;

		struct WindowData
		{
			std::string Title;
			uint32_t width, height;
			bool VSync, MSAA;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};

}