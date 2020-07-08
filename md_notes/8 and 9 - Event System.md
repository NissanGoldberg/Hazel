### 8-9 - Event System

* The `window` class **shouldn't** have any knowledge of the `Application` Class
* Application creates Window
* We have to message back to Application
* We need an Event Class, so if a click we need (x pos, y pos, button_type) then we send it back to Application.
* We should provide window with an ***event callback***. So everytime the window gets an event which isn't null it will call the callback with the event data. And will call `OnEvent(Event& e)` with the data
* This is called a **Blocking Event** since it calls as soon as it receives the event (there is also a *buffered event system*)
* EventListenerInterface and window can have an array of I_event_listerner and iterate through them.
* Event Dispatcher

![](C:\Users\nissa\OneDrive\Desktop\Coding\CPP\Game Engine\8 - Planning the Event System.png)

![](C:\Users\nissa\OneDrive\Desktop\Coding\CPP\Game Engine\8 - Planning the Event System-2.png)

 

### Video 9 - Event System



```lua
project "Hazel"
    ...
	includedirs {
		"%{prj.name}/src",        
		"%{prj.name}/vendor/spdlog/include"
	}
```

Added `"%{prj.name}/src"  ` so that in  `Hazel/Events/Hazel.h`

we can use include as so, instead of going back and then going into Core.h, but rather relative to `src`

```cpp
#include "Hazel/Core.h"
...
```

Watch [here](https://youtu.be/xnopUoZbMEk?t=1799)

Also watch from there the differences between the files





### Files

```cpp
//Event.h
#pragma once

#include "Hazel/Core.h"

#include <string>
#include <functional>


namespace Hazel {

	// Events in Hazel are currently blocking, meaning when an event occurs it
	// immediately gets dispatched and must be dealt with right then an there.
	// For the future, a better strategy might be to buffer events in an event
	// bus and process them during the "event" part of the update stage.

	//Each event type is split on different line
	//This will prevent dynamic casting
	enum class EventType{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	//BIT field (defined in Core.h) - is a macro, that left shifts 1 a num of times.
	//This helps if we only lets say care about mouse events so give us all the mouse events
	// Have to ability to have mulitple catergories to a single event type
	enum EventCategory{
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4)
	};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	//Base class for events
	class HAZEL_API Event{
		friend class EventDispatcher;
	protected:
		bool Handled = false; //Check if event was handled or not
		//Ex. Mouse click event falls in boundary we want to consume and thats it
	public:
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0; //Only for debugging, is overhead
		virtual int GetCategoryFlags() const = 0;

		//Ex. on resize print width and height
		//This is also only for DEBUGGING, impacts performance
		virtual std::string ToString() const { return GetName(); }

		//Is the given event in a certain category, 
		// we can filter out certain events
		inline bool IsInCategory(EventCategory category){
			//If we return 0, not a category at all, otherwise true and in that category
			return GetCategoryFlags() & category;
		}
	};


	class EventDispatcher{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event)
			: m_Event(event)
		{
		}

		// T can be any event type
		//Ex. takes in WindowResizeEvent and returns a boolean
		template<typename T>
		bool Dispatch(EventFn<T> func){
			if (m_Event.GetEventType() == T::GetStaticType()){
				m_Event.Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};


	//For logging library
	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}

}
```

```cpp
//ApplicationsEvent.h
#pragma once

#include "Event.h"

#include <sstream>

namespace Hazel {

	class HAZEL_API WindowResizeEvent : public Event{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_Width(width), m_Height(height) {}

		unsigned int GetWidth() const { return m_Width; }
		unsigned int GetHeight() const { return m_Height; }

		std::string ToString() const override{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		unsigned int m_Width, m_Height;
	};

	class HAZEL_API WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent(){}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class HAZEL_API AppTickEvent : public Event
	{
	public:
		AppTickEvent() {}

		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class HAZEL_API AppUpdateEvent : public Event{
	public:
		AppUpdateEvent() {}

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class HAZEL_API AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() {}

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
}
```

```cpp
//KeyEvent.h
#pragma once

#include "Event.h"

#include <sstream>

namespace Hazel {

	/**
	* A Key Event is when we press a key on keyboard,
	*	Also checks if it is an repeated event.
	*	What happens is when a key is pressed the OS sends a KeyEvent
	*	And after a few seconds will send key repeat event.
	* When we release this is a key release event.
	* KeyEvent is more like an abstract class
	*/
	class HAZEL_API KeyEvent : public Event{
	public:
		inline int GetKeyCode() const { return m_KeyCode; }

		
		/**
		* MACRO (in Event.h)
		* implements the virtual functions:
		*
		*		virtual EventType GetEventType() const = 0;
		*		virtual const char* GetName() const = 0; 
		*		virtual int GetCategoryFlags() const = 0;
		*
		*       as so:
		*
		*      static EventType GetStaticType() { return EventType::Keypressed; }\
		*	   virtual EventType GetEventType() const override { return GetStaticType(); }\
		*	   virtual const char* GetName() const override { return "KeyPressed"; }
		*/
		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		//protected ctor, because nothing else can construct it else those
		//who derive from it
		KeyEvent(int keycode)
			: m_KeyCode(keycode) {}

		int m_KeyCode;
	};

	//Actual event
	class HAZEL_API KeyPressedEvent : public KeyEvent {
	public:
		KeyPressedEvent(int keycode, int repeatCount)
			: KeyEvent(keycode), m_RepeatCount(repeatCount) {}

		inline int GetRepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		int m_RepeatCount;
	};

	class HAZEL_API KeyReleasedEvent : public KeyEvent {
	public:
		KeyReleasedEvent(int keycode)
			: KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class HAZEL_API KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(int keycode)
			: KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};
}
```

```cpp
//MouseEvent.h
#pragma once

#include "Event.h"

#include <sstream>

namespace Hazel {

	class HAZEL_API MouseMovedEvent : public Event{
	public:
		MouseMovedEvent(float x, float y)
			: m_MouseX(x), m_MouseY(y) {}

		float GetX() const { return m_MouseX; }
		float GetY() const { return m_MouseY; }

		std::string ToString() const override{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float m_MouseX, m_MouseY;
	};

	class HAZEL_API MouseScrolledEvent : public Event{
	public:
		MouseScrolledEvent(float xOffset, float yOffset)
			: m_XOffset(xOffset), m_YOffset(yOffset) {}

		float GetXOffset() const { return m_XOffset; }
		float GetYOffset() const { return m_YOffset; }

		std::string ToString() const override{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float m_XOffset, m_YOffset;
	};


	//Base class for MouseButtonPressedEvent and MouseButtonReleasedEvent
	class HAZEL_API MouseButtonEvent : public Event{
	public:
		inline int GetMouseButton() const { return m_Button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	protected:
		//Protected so we cant create this event
		MouseButtonEvent(int button)
			: m_Button(button) {}

		int m_Button;
	};

	class HAZEL_API MouseButtonPressedEvent : public MouseButtonEvent{
	public:
		MouseButtonPressedEvent(int button)
			: MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class HAZEL_API MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int button)
			: MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};

}
```

