#pragma once
#include <string>

// GameObject and Component are a subclass of GameSystem::Object.
// It allows the system to pass values in an abstract value to encapsulate includes.

#undef SendMessage

namespace GameSystem
{
	class Object
	{
	public:
		virtual ~Object()												= default;
		[[nodiscard]] virtual int GetID() const							= 0;
		[[nodiscard]] virtual std::string GetTypeName() const			= 0;
		virtual bool SendMessage(void* aMessage, unsigned int aChannel)	= 0;

	protected:

	private:

	};
}

