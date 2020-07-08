#pragma once
#include "Core.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/Event.h"
#include "Hazel/LayerStack.h"  
#include "Window.h"

//#include <GLFW/glfw3.h> //<-- Added this vid 14
						
namespace Hazel {

	class HAZEL_API Application {
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);   //<-- Added this vid 13
		void PushOverlay(Layer* layer); //<-- Added this vid 13
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;        //<-- Added this vid 13
	};

	//To be defined in client
	Application* CreateApplication();
}