# 10 - Precompiled Headers



first lets add `hzpch.h` (hazel precompiled header) and `hzpch.cpp` under the `src` folder

```cpp
//hzpch.h
#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#ifdef HZ_PLATFORM_WINDOWS
	#include <Windows.h>
#endif // HZ_PLATFORM_WINDOWS
```

```cpp
//hzpch.cpp
#include "hzpch.h"
```



now we can remove the headers from our files

`Event.h`

```
//Event.h
#pragma once
#include "hzpch.h"       // <--- Added this
#include "Hazel/Core.h"

//#include <string>       //<---removed this
//#include <functional>   //<---removed this
```

`KeyEvent.h`

```cpp
#pragma once
#include "hzpch.h"   // <--- Added this
#include "Event.h"

//#include <sstream> //<---removed this
```

`Log.h` 

```cpp
#pragma once
#include "hzpch.h"  // <--- Added this
//#include <memory> //<---removed this
```

How we have to add our precompiled header to **premake**

```lua
project "Hazel"
	...
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir("bin-int/" .. outputdir .. "/%{prj.name}")
	
	pchheader "hzpch.h"
	pchsource "Hazel/src/hzpch.cpp"
```

` pchsource` must be given the entire path



Now add to every single `.cpp` file **before anything else**!

```
//Log.cpp
#include "hzpch.h"

#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
...
```

```cpp
//Application.cpp
#include "hzpch.h"

#include "Application.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Log.h"
...
```

