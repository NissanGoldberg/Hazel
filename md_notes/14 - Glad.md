# 14 - Layers

Files Changed

* `premake5.lua` Hazel 
* `WindowsWindow.cpp`



Files Created

* `premake5.lua` Glad



Hazel premake

```lua
...
-- Include directories relative to root folder (solution dir)
IncludeDir = {}
IncludeDir["GLFW"] = "Hazel/vendor/GLFW/include"
IncludeDir["Glad"] = "Hazel/vendor/Glad/include"

include "Hazel/vendor/GLFW"
include "Hazel/vendor/Glad"

...

includedirs {
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}"
	}
	
	links {
		"GLFW",
		"Glad",
		"opengl32.lib"
	}

    filter "system:windows"
            cppdialect "C++17"
            staticruntime "On"
            systemversion "latest"

            defines {
                "HZ_PLATFORM_WINDOWS",
                "HZ_BUILD_DLL",
                "GLFW_INCLUDE_NONE"
            }
```

GLAD premake

```lua
project "Glad"
    kind "StaticLib"
    language "C"
    staticruntime "on"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files{
        "include/glad/glad.h",
        "include/KHR/khrplatform.h",
        "src/glad.c"
    }

    includedirs{
        "include"
    }
    
    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
```

```cpp
//Application.cpp
#include "hzpch.h"

#include "Application.h"
#include "Hazel/Log.h"

#include <glad/glad.h>
```



```cpp
//WindowsWindow.cpp
#include "hzpch.h"

#include "Application.h"
#include "Hazel/Log.h"

#include <glad/glad.h>

namespace Hazel {
    ...
        
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
            
        //======Added in vid 13==========
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		HZ_CORE_ASSERT(status, "Failed to initialize Glad!");
        ...
```

```cpp
//Application.h
#include "Core.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/Event.h"
#include "Hazel/LayerStack.h"  
#include "Window.h"

//#include <GLFW/glfw3.h> //<-- Added this vid 14
...
```

