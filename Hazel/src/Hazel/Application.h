#pragma once
#include "Core.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/Event.h"
#include "Hazel/LayerStack.h"  
#include "Window.h"

#include "Hazel/ImGui/ImGuiLayer.h"
						
namespace Hazel {

	class HAZEL_API Application {
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer); 
		void PushOverlay(Layer* layer); 

		inline static Application& Get() { return *s_Instance;  } //<-- Added this vid 15
		inline Window& GetWindow() { return *m_Window; } //<-- Added this vid 15
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;  

		unsigned int m_VertexArray, m_VertexBuffer, m_IndexBuffer;
	private:
		static Application* s_Instance; //<-- Added this vid 15, SINGLETON
	};

	//To be defined in client, in our code is defined in SandboxApp
	Application* CreateApplication();
}