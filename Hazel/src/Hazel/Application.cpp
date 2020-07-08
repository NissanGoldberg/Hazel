#include "hzpch.h"

#include "Application.h"
#include <GLFW/glfw3.h>
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

	//Added in vid 13
	void Application::PushLayer(Layer* layer) {
		m_LayerStack.PushLayer(layer);
	}

	//Added in vid 13
	void Application::PushOverlay(Layer* layer) {
		m_LayerStack.PushOverlay(layer);
	}

	//Changed in vid 13
	void Application::OnEvent(Event& e) {
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		HZ_CORE_TRACE("{0}", e); //macro in Log.h , for debugging

		//Added in vid 13
		//Iterate backwards
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); ) {
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	//Changed in vid 13
	void Application::Run() {

		while (m_Running) {
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			m_Window->OnUpdate();
		}

	}

	// Added in vid 12
	bool Application::OnWindowClose(WindowCloseEvent& e) {
		m_Running = false;
		return true;
	}
}