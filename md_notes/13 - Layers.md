# 13 - Layers


When we want to draw something to a screen we will have layers. This also applies to logic when updating. Every layer in the **layer stack** will be updated in the update according to the stack. The layer stack is very useful for also **overlays**.

Useful for events. With events it is the **opposite**, the last rendered layer should receive the event.

Also we can have a debug layer that displays hit boxes etc. Also we can have a UI layer.

So basically it 

* renders
* receives events



Files Changed

* `Application.cpp` and `Application.h`
* `SandboxApp.cpp`
* `premake5.lua`

New Files

* `LayerStack.cpp` and `LayerStack.h`
* `Layer.cpp` and `Layer.h`



## New Files

```cpp
//Layer.h
#pragma once

#include "Hazel/Core.h"
#include "Hazel/Events/Event.h"

namespace Hazel {

	class HAZEL_API Layer{
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
```

Layer.cpp

```cpp
//Layer.cpp
#include "hzpch.h"
#include "Layer.h"

namespace Hazel {

	Layer::Layer(const std::string& debugName)
		: m_DebugName(debugName)
	{
	}
}
```

LayerStack.h

```cpp
#pragma once

#include "Hazel/Core.h"
#include "Layer.h"

#include <vector>

namespace Hazel {

	class HAZEL_API LayerStack{
	public:
		//LayerStack() = default;
		LayerStack();
        
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }
	private:
		std::vector<Layer*> m_Layers;
		std::vector<Layer*>::iterator m_LayerInsert;
	};
}
```

LayerStack.cpp

```cpp
#include "hzpch.h"
#include "LayerStack.h"

namespace Hazel {

	LayerStack::LayerStack() {
		m_LayerInsert = m_Layers.begin();
	}

	LayerStack::~LayerStack(){
		for (Layer* layer : m_Layers)
			delete layer;
	}

	void LayerStack::PushLayer(Layer* layer){
		m_Layers.emplace(m_LayerInsert, layer);
	}

	void LayerStack::PushOverlay(Layer* overlay) {
		m_Layers.emplace_back(overlay);
	}

    void LayerStack::PopLayer(Layer* layer) {
            auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
            if (it != m_Layers.end()) {
                m_Layers.erase(it);
                m_LayerInsert--; //should be prefix
            }
        }

	void LayerStack::PopOverlay(Layer* overlay){
		auto it = std::find(m_Layers.begin(), m_Layers.end(), overlay);
		if (it != m_Layers.end())
			m_Layers.erase(it);
	}
}
```

**Layers** get pushed in the first half of the list and **Overlays** get pushed in the 2nd half of the list, because overlays we always want to render last.



## Changed Files

Application.h

```cpp
#pragma once
#include "Core.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/Event.h"//<-- Added this vid 13
#include "Hazel/LayerStack.h"  //<-- Added this vid 13
#include "Window.h"

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
```

Application.cpp

```cpp
#include "hzpch.h"

#include "Application.h"
#include <GLFW/glfw3> //try #include <GLFW/glfw3.h> instead
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
	void Application::PushLayer(Layer* layer){
		m_LayerStack.PushLayer(layer);
	}

	//Added in vid 13
	void Application::PushOverlay(Layer* layer){
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
```

Also add `#include "Hazel/Layer.h"` to `Hazel.h`



#### Note

in `Event.h` I changed the `bool Handled = false;` from protected to public



### Premake

```lua
project "Hazel"
	...
	filter "configurations:Debug"
		defines "HZ_DEBUG"
		buildoptions "/MDd" --- Added this
		symbols "On"
		
	filter "configurations:Release"
		defines "HZ_RELEASE"
		buildoptions "/MD" --- Added this
		optimize "On"
	
	filter "configurations:DIST"
		defines "HZ_DIST"
		buildoptions "/MD" --- Added this
		optimize "On"

project "Sandbox"
	...
	filter "configurations:Debug"
		defines "HZ_DEBUG"
		buildoptions "/MDd" --- Added this
		symbols "On"
		
	filter "configurations:Release"
		defines "HZ_RELEASE"
		buildoptions "/MD" --- Added this
		optimize "On"
	
	filter "configurations:DIST"
		defines "HZ_DIST"
		buildoptions "/MD" --- Added this
		optimize "On"
```

Multi-threaded Debug DLL (MDd)



SandboxApp.cpp

```cpp
#include <Hazel.h>

//Added in video13
class ExampleLayer : public Hazel::Layer {
public:
	ExampleLayer() : Layer("Example") {}
	void OnUpdate() override {
		HZ_INFO("ExampleLayer::Update");
	}

	void OnEvent(Hazel::Event& event) override {
		HZ_TRACE("{0}", event);
	}
};

class Sandbox : public Hazel::Application {
public:
	Sandbox(){
		PushLayer(new ExampleLayer());
	}

	~Sandbox() {}
};


Hazel::Application* Hazel::CreateApplication() {
	return new Sandbox;
}
```



### Git

```
git checkout vid13Layers
```


```
git push -u origin vid13Layers
```