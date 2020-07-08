# 11 - Window Abstraction and GLFW

GLFW - cross platform - allows windowing, doesn't support DirectX for that we need Win32

We will implement a Window class **per platform** (most will be the same)

Made a Platform directory and a Windows dir inside

```
> git submodule add https://github.com/TheCherno/glfw Hazel/vendor/GLFW
```

Has a premake file

Now we want to add to our premake file the following

```lua
-- Include directories relative to root folder (solution dir)
IncludeDir = {}
IncludeDir["GLFW"] = "Hazel/vendor/GLFW/include"

include "Hazel/vendor/GLFW"

project "Hazel"
	...

	includedirs {
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}"
	}
	
	links {
		"GLFW",
		"opengl32.lib"
	}
```

`include "Hazel/vendor/GLFW"` will include the premake file

### Window Class

```cpp
//Window.h
#pragma once

#include "hzpch.h"

#include "Hazel/Core.h"
#include "Hazel/Events/Event.h"

namespace Hazel {

	struct WindowProps{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title = "Hazel Engine",
			uint32_t width = 1280,
			uint32_t height = 720)
			: Title(title), Width(width), Height(height)
		{
		}
	};

	// Interface representing a desktop system based Window
    //Is an interface since it has to implemented per platform
	class HAZEL_API Window{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {};

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

        //Create has to implemented per platform
		static Window* Create(const WindowProps& props = WindowProps());
	};
}
```



Now add `Platform/Windows` dirs in `src`

and create `WindowsWindow.cpp/h` inside the ``Platform/Windows`` dir



### WindowsWindow

```cpp
//WindowsWindow.h
#pragma once

#include "Hazel/Window.h"
#include <GLFW/glfw3.h>

namespace Hazel {

	class WindowsWindow : public Window {
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		unsigned int GetWidth() const override { return m_Data.Width; }
		unsigned int GetHeight() const override { return m_Data.Height; }

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;

        //This struct is to pass into GLFW and not the entire class
		//This is where we store data before callbacks
		struct WindowData{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};

}
```

```cpp
//WindowsWindow.cpp
#include "hzpch.h"
#include "WindowsWindow.h"


namespace Hazel {

	static bool s_GLFWInitialized = false;

    //Notice this is Window::Create not WindowsWindow::Create
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
		HZ_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		if (!s_GLFWInitialized) {
			int success = glfwInit();
            //Macro, check our modified Core.h
			HZ_CORE_ASSERT(success, "Could not initialize GLFW!"); 

			s_GLFWInitialized = true;
		}

		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_Data); //m_Data struct we place in callback function
		SetVSync(true);
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

And our modified `Core.h`

```cpp
//Core.h
#pragma once

#ifdef HZ_PLATFORM_WINDOWS
	#ifdef HZ_BUILD_DLL
		#define HAZEL_API __declspec(dllexport)
	#else
		#define HAZEL_API __declspec(dllimport)
	#endif // HZ_BUILD_DLL
#else
	#error Hazel only supports Windows!
#endif // HZ_PLATFORM_WINDOWS


#ifdef HZ_ENABLE_ASSERTS
	#define HZ_ASSERT(x, ...) { if(!(x)) { HZ_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define HZ_CORE_ASSERT(x, ...) { if(!(x)) { HZ_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define HZ_ASSERT(x, ...) 
	#define HZ_CORE_ASSERT(x, ...)
#endif // HZ_ENABLE_ASSERTS

#define BIT(x) (1 << x)
```



### Now add to Application

```cpp
//Application.h
#pragma once
#include "Core.h"
#include "Events/Event.h"
#include "Window.h"

namespace Hazel {

	class HAZEL_API Application {
	public:
		Application();
		virtual ~Application();

		void Run();
	private:
		std::unique_ptr<Window> m_Window; // <---- Added this
	};

	//To be defined in client
	Application* CreateApplication();
}
```

and in our 

```cpp
//Application.cpp
#include "Application.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Log.h"


namespace Hazel {

	Application::Application() {
		m_Window = std::unique_ptr<Window>(Window::Create());
	}

	Application::~Application() {
	}

	void Application::Run() {

		while (m_Running) {
			m_Window->OnUpdate();
		}

	}
}
```

Application is a singleton
