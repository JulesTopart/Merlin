#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Events/Event.h"

namespace Merlin {

	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;
		bool VSync, MSAA;

		WindowProps(const std::string& title = "OpenGL Sandbox",
			        uint32_t width = 1280,
			        uint32_t height = 720, bool vsync = false, bool multisampling = false )
			: Title(title), Width(width), Height(height), MSAA(multisampling), VSync(vsync)
		{
		}
	};

	// Interface representing a desktop system based Window
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Window* Create(const WindowProps& props = WindowProps());
	};

}