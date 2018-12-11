#pragma once

#include "GameObject.h"
#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/State.h"

namespace NCL
{
	namespace CSC8503
	{
		class Robot : public GameObject
		{
		public:
			Robot(string name = "Robot");
			~Robot();

			void OnCollisionBegin(GameObject* otherObject);
			void OnCollisionEnd(GameObject* otherObject);

			void InitialiseStates();
			void InitialiseStateFunctions();
			void InitialiseStateTransitions();

			StateMachine* GetStateMachine() const { return stateMachine; }

			bool IsPlayerInRange() const { return playerInRange; }
			void SetPlayerInRange(bool b) { playerInRange = b; }

		protected:
			StateMachine* stateMachine;

			GenericState* robotStateDefault;
			GenericState* robotStateChase;

			StateFunc Default;
			StateFunc Chase;

			GenericTransition<bool&, bool>* transitionDefaultToChase;
			GenericTransition<bool&, bool>* transitionChaseToDefault;

			bool playerInRange;
		};
	};
};
