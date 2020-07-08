### Build System - Premake

[Premake link](https://github.com/premake/premake-core)

`premake5.lua`

```lua
-- premake5.lua
workspace "Hazel"
	architecture "x64"
	
	configurations {
		"Debug",
		"Release",
		"Dist"
	}
	
outputdir = "%{cfg.builcfg}-%{cfg.system}-%{cfg.architecture}"	

project "Hazel"
	location "Hazel"
	kind "SharedLib"
	language "C++"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir("bin-int/" .. outputdir .. "/%{prj.name}")
	
	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	
	includedirs {
		"%{prj.name}/vendor/spdlog/include"
	}
	
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"
		
		defines {
			"HZ_PLATFORM_WINDOWS",
			"HZ_BUILD_DLL"
		}
		
		postbuildcommands {
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}
	
	filter "configurations:Debug"
		defines "HZ_DEBUG"
		symbols "On"
		
	filter "configurations:Release"
		defines "HZ_RELEASE"
		optimize "On"
	
	filter "configurations:DIST"
		defines "HZ_DIST"
		optimize "On"

		
project "Sandbox"
	location "Hazel"
	kind "ConsoleApp"
	language "C++"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir("bin-int/" .. outputdir .. "/%{prj.name}")
	
	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	
	includedirs {
		"Hazel/vendor/spdlog/include",
		"Hazel/src"
	}
	
	links  {"Hazel"}
	
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"
		
		defines {
			"HZ_PLATFORM_WINDOWS"
		}
		
	
	filter "configurations:Debug"
		defines "HZ_DEBUG"
		symbols "On"
		
	filter "configurations:Release"
		defines "HZ_RELEASE"
		optimize "On"
	
	filter "configurations:DIST"
		defines "HZ_DIST"
		optimize "On"
```



```
> C:\dev\Hazel> .\vendor\bin\premake\premake5.exe vs2019
```

Run from lua file

and easier solution is a .bat file

GenerateProjects.bat

```bat
call vendor\bin\premake\premake5.exe vs2019
PAUSE
```

double click, reload then F5 and done