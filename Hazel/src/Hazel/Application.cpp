#include "hzpch.h"

#include "Application.h"

#include "Hazel/Log.h"


namespace Hazel {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application::Application() {
		m_Window = std::unique_ptr<Window>(Window::Create());
		
		//Added in vid 12
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
		// macro for std::bind(&Application::OnEvent, this, std::placeholders::_1
	}

	Application::~Application() {
	}

	//Added in vid 12
	void Application::OnEvent(Event& e) {
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		HZ_CORE_TRACE("{0}", e); //macro in Log.h , for debugging
	}


	void Application::Run() {

		while (m_Running) {
			m_Window->OnUpdate();
		}

	}

	// Added in vid 12
	bool Application::OnWindowClose(WindowCloseEvent& e) {
		m_Running = false;
		return true;
	}
}
