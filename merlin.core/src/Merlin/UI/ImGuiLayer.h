#pragma once
#include "Merlin/Core/Core.h"

#include "Merlin/Core/Layer.h"
#include "Merlin/Events/ApplicationEvent.h"
#include "Merlin/Events/KeyEvent.h"
#include "Merlin/Events/MouseEvent.h"

namespace Merlin {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void onAttach() override;
		virtual void onDetach() override;

		void begin();
		void end();

		virtual void onEvent(Event& event);
		bool onMouseButtonPressed(MouseButtonPressedEvent& e);
	private:
		float m_Time = 0.0f;
	};

}