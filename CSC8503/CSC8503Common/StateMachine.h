#pragma once
#include <vector>
#include <map>

namespace NCL
{
	namespace CSC8503
	{
		class State;
		class StateTransition;

		typedef std::multimap<State*, StateTransition*> TransitionContainer;
		typedef TransitionContainer::iterator TransitionIterator;

		class StateMachine
		{
		public:
			StateMachine();
			~StateMachine();

			void AddState(State* s);
			void AddTransition(StateTransition* t);

			void Update();

			State* GetActiveState() const { return activeState; }
			TransitionContainer GetAllTransitions() const { return allTransitions; }

		protected:
			State * activeState;

			std::vector<State*> allStates;

			TransitionContainer allTransitions;
		};
	}
}