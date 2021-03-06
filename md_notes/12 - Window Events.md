# 12 - Window Events

We need a **close** button and resize events

Files Changed

* `Application.cpp`
* `Application.h`
* `WindowsWindow.cpp`

```cpp
//Application.cpp
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
```

```cpp
//Application.h
#pragma once
#include "Core.h"
#include "Events/Event.h"
#include "Hazel/Events/ApplicationEvent.h"

#include "Window.h"

namespace Hazel {

	class HAZEL_API Application {
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e); // Added in vid 12
	private:
		bool OnWindowClose(WindowCloseEvent& e); // Added in vid 12
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
	};

	//To be defined in client
	Application* CreateApplication();
}


```

```cpp
//WindowsWindow.cpp
#include "hzpch.h"
#include "WindowsWindow.h"


#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/MouseEvent.h"
#include "Hazel/Events/KeyEvent.h"




namespace Hazel {

	static bool s_GLFWInitialized = false;

	//Added in vid 12
	static void GLFWErrorCallback(int error, const char* description) {
		//not defined yet: HZ_CORE_ERROR
		//HZ_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Window* Window::Create(const WindowProps& props){
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props) {
		Init(props);
	}

	WindowsWindow::~WindowsWindow() { 
		Shutdown();
	}


	void WindowsWindow::Init(const WindowProps& props) {
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		//Not yet implemented
		//HZ_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		if (!s_GLFWInitialized) {
			int success = glfwInit();
			HZ_CORE_ASSERT(success, "Could not initialize GLFW!");

			//Added in vid 12
			glfwSetErrorCallback(GLFWErrorCallback);

			s_GLFWInitialized = true;
		}

		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_Data); //m_Data struct we place in callback function
		SetVSync(true);

		//Added in callbacks vid 12
		//Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
			
			});


		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
					WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
					WindowCloseEvent event;
					data.EventCallback(event);
				});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				
				switch (action) {
					case GLFW_PRESS: {
						KeyPressedEvent event(key, 0);
						data.EventCallback(event);
						break;
						
					}
					case GLFW_RELEASE: {
						KeyReleasedEvent event(key);
						data.EventCallback(event);
						break;

					}
					case GLFW_REPEAT: {
						KeyPressedEvent event(key, 1);
						data.EventCallback(event);
						break;

					}
				}
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			
			switch (action) {
				case GLFW_PRESS: {
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;

				}
				case GLFW_RELEASE: {
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;

				}
			}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset){
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos){
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseMovedEvent event((float)xPos, (float)yPos);
				data.EventCallback(event);
			});
	}

	void WindowsWindow::Shutdown(){
		glfwDestroyWindow(m_Window);
	}

	void WindowsWindow::OnUpdate(){
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void WindowsWindow::SetVSync(bool enabled){

		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const{
		return m_Data.VSync;
	}
}
```



### Note :

Window closing freezes