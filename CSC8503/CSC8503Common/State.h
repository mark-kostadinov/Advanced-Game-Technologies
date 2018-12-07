#pragma once

#include "definitions.h"

namespace NCL
{
	namespace CSC8503
	{
		class State
		{
		public:
			State(std::string name = "");
			virtual ~State();
			virtual void Update() = 0; // Pure virtual base class

			std::string GetName() const { return name; }

		protected:
			std::string name;
		};

		typedef void(*StateFunc)(void*);

		class GenericState : public State
		{
		public:
			GenericState(StateFunc someFunc, void* someData, std::string name = "")
			{
				func = someFunc;
				funcData = someData;
				this->name = name;
			}
			virtual void Update()
			{
				if (funcData != nullptr)
				{
					func(funcData);
				}
			}
		protected:
			StateFunc func;
			void* funcData;
		};
	}
}