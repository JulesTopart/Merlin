#pragma once
#include "merlin/core/core.h"
#include "merlin/events/event.h"

namespace Merlin {

	enum MSAA_PRESET
	{
		LOW, MEDIUM, HIGH, ULTRA
	};


	struct WindowProps
	{
		std::string Title;
		uint32_t width;
		uint32_t height;
		bool VSync, MSAA, FullScreen;

		WindowProps(const std::string& title = "Merlin Engine",
			        uint32_t width = 1280,
			        uint32_t height = 720, 
					bool vsync = false, 
					bool multisampling = false, 
					bool fullscreen = false)
			: Title(title), width(width), height(height), MSAA(multisampling), VSync(vsync), FullScreen(fullscreen)
		{
		}
	};

	// Interface representing a desktop system based Window
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void onUpdate() = 0;

		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;

		// Window attributes
		virtual void setEventCallback(const EventCallbackFn& callback) = 0;
		virtual void setVSync(bool enabled) = 0;
		virtual bool isVSync() const = 0;

		virtual void* getNativeWindow() const = 0;

		static Window* create(const WindowProps& props = WindowProps());
	};

}