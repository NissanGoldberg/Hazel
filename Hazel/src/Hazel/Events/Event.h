#pragma once
#include "hzpch.h"

#include "Hazel/Core.h"


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

	public:
		bool Handled = false; //Check if event was handled or not
//Ex. Mouse click event falls in boundary we want to consume and thats it

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