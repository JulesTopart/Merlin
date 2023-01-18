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

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void Begin();
		void End();

		virtual void ImGuiLayer::OnEvent(Event& event);
		bool ImGuiLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e);
	private:
		float m_Time = 0.0f;
	};

}