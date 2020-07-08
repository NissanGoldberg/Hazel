[Handmadehero](https://www.youtube.com/user/handmadeheroarchive/playlists)

### Game Engine

A data transformation machine. Here we will implement these features

* **entry point**- what happens the code controls or the game controls, what control what happens
* **application layer** - what keeps the loops, events, resizing
* **window** - on mobile its more like a surface. But on desktop its the window
  * **Input** - 
  * **Events** - event manager, its a messaging system that alerts when something happens.
* **Render** - most complicated (later on)
* **Render API abstraction** - Opengl (simplest platform). DirectX is more complicated.
* **Debugging support** - good ways to see whats going on like a ***logging system,*** ***profiler***
* **Scripting language** like lua,python
* **Memory Systems**
* **Entity component system** - physics
* **Physics**
* **File IO, VFS**
* **Build System**





### Project Setup

With dynamic libraries. we can link everything into a single `dll` file and link to it. We link all our static libraries into our dll and then link our dll into our game

* now we'll remove x86 and win32
* change config type to dll in all configurations
* output: `$(SolutionDir)bin\$(Configuration)-$(Platform)\$(ProjectName)\`
* 





```cpp
//Test.h
#pragma once

namespace Hazel {
	
	__declspec(dllexport) void Print(); //<--export
}
```

Application.cpp

```cpp
namespace Hazel {
	__declspec(dllimport) void Print(); //<--import
}

void main() {
	Hazel::Print();
}
```

Unfortunately, in the mean time we have to place the dll file in the Sandbox dir



### Entry Point -[link](https://www.youtube.com/watch?v=meARMOmTLgE&list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT&index=5)

```cpp
//Application.h
#pragma once

namespace Hazel {

	class __declspec(dllexport) Application {
	public:
		Application();
		virtual ~Application();

		void Run();
	};
}

```

Let make a **macro** so that we dont have to type `__declspec(dllexport)` and `import` each time



### Now we get this

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
```

```cpp
//Application.h
#pragma once
#include "Core.h"

namespace Hazel {

	class HAZEL_API Application {
	public:
		Application();
		virtual ~Application();

		void Run();
	};
}
```

```c++
//Application.cpp
#include "Application.h"

namespace Hazel {
	Application::Application() {}

	Application::~Application() {}

	void Application::Run(){
		while (true);
	}
}
```



### Finally we get this

```cpp
//Sandbox/src/SandboxApp.cpp
#include <Hazel.h>

class Sandbox : public Hazel::Application {
public:
	Sandbox(){}

	~Sandbox() {}
};


Hazel::Application* Hazel::CreateApplication() {
	return new Sandbox;
}
```

```cpp
//Hazel/Haze.h
#pragma once

//For use by Hazel Applications
#include "Hazel/Application.h"
#include <iostream>

//====== Entry Point =======
#include "Hazel/EntryPoint.h"
```

```cpp
// Hazel/src/Hazel/Application.h
#pragma once
#include "Core.h"

namespace Hazel {

	class HAZEL_API Application {
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	//To be defined in client
	Application* CreateApplication();
}
```

```cpp
// Hazel/src/Hazel/EntryPoint.h
#pragma once

#ifdef HZ_PLATFORM_WINDOWS

extern Hazel::Application* Hazel::CreateApplication();

int main(int argc, char** argv) {
	std::cout << "Hazel Engine\n";
	auto app = Hazel::CreateApplication();
	app->Run();
	delete app;
}

#endif // HZ_PLATFORM_WINDOWS
```

`extern` - This comes in useful when you have **global variables.** You declare the *existence* of global variables in a header, so that each source file that includes the header knows about it, but you only need to “define” it once in one of your source files.

To clarify, using `extern int x;` tells the compiler that an object of type `int` called `x` exists *somewhere*. It's not the compilers job to know where it exists, it just needs to know the type and name so it knows how to use it. Once all of the source files have been compiled, the linker will resolve all of the references of `x` to the one definition that it finds in one of the compiled source files. For it to work, the definition of the `x` variable needs to have what's called “external linkage”, which basically means that it needs to be declared outside of a function (at what's usually called “the file scope”) and without the `static` keyword.

### header:

```cpp
#ifndef HEADER_H
#define HEADER_H

// any source file that includes this will be able to use "global_x"
extern int global_x;

void print_global_x();

#endif
```

### source 1:

```cpp
#include "header.h"

// since global_x still needs to be defined somewhere,
// we define it (for example) in this source file
int global_x;

int main(){
    //set global_x here:
    global_x = 5;

    print_global_x();
}
```

### source 2:

```cpp
#include <iostream>
#include "header.h"

void print_global_x(){
    //print global_x here:
    std::cout << global_x << std::endl;
}
```