#pragma once
#include "merlin/core/core.h"

#include "merlin/core/layer.h"
#include "merlin/events/applicationevent.h"
#include "merlin/events/keyevent.h"
#include "merlin/events/mouseevent.h"

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