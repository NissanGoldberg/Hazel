//Layer.h
#pragma once

#include "Hazel/Core.h"
#include "Hazel/Events/Event.h"

namespace Hazel {

	class HAZEL_API Layer {
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() = default;

		virtual void OnAttach() {} //When adding to the layer stack, we attach it
		virtual void OnDetach() {}
		virtual void OnUpdate() {} //Get called when the App updates
		virtual void OnEvent(Event& event) {}

		const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName; //For Debug
	};
}
