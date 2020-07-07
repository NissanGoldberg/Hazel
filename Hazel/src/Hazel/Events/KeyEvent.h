#pragma once
#include "hzpch.h"

#include "Event.h"

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