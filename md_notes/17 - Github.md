# 17 - Github

Hazel premake

```lua
project "Hazel"
	location "Hazel"
	kind "SharedLib"
	language "C++"
	staticruntime "off"
	
    ...
    filter "configurations:Debug"
            defines "HZ_DEBUG"
            runtime "Debug"
            symbols "On"

        filter "configurations:Release"
            defines "HZ_RELEASE"
            runtime "Release"
            optimize "On"

        filter "configurations:DIST"
            defines "HZ_DIST"
            runtime "Release"
            optimize "On"

project "Sandbox"
	...
	staticruntime "off"

	...
    filter "configurations:Debug"
            defines "HZ_DEBUG"
            runtime "Debug"
            symbols "On"

        filter "configurations:Release"
            defines "HZ_RELEASE"
            runtime "Release"
            optimize "On"

        filter "configurations:DIST"
            defines "HZ_DIST"
            runtime "Release"
            optimize "On"
```

