#include "Robot.h"

NCL::CSC8503::Robot::Robot(string name)
{
	this->name = name;

	stateMachine = new StateMachine();
	playerInRange = false;

	InitialiseStates();
}

NCL::CSC8503::Robot::~Robot()
{
	delete stateMachine;
	stateMachine = NULL;
}

void NCL::CSC8503::Robot::OnCollisionBegin(GameObject * otherObject)
{
	if (otherObject->GetName() == "Floor")
		GetPhysicsObject()->SetFriction(0.5f);
	if (otherObject->GetName() == "Sand")
		GetPhysicsObject()->SetFriction(0.001f);
}

void NCL::CSC8503::Robot::OnCollisionEnd(GameObject * otherObject)
{
	GetPhysicsObject()->SetFriction(0.95f);
}

void NCL::CSC8503::Robot::InitialiseStates()
{
	if (!stateMachine)
		return;

	InitialiseStateFunctions();

	robotStateDefault = new GenericState(Default, this, "Default");
	robotStateChase = new GenericState(Chase, this, "Chase");

	stateMachine->AddState(robotStateDefault);
	stateMachine->AddState(robotStateChase);

	InitialiseStateTransitions();

	stateMachine->AddTransition(transitionDefaultToChase);
	stateMachine->AddTransition(transitionChaseToDefault);
}

void NCL::CSC8503::Robot::InitialiseStateFunctions()
{
	Default = [](void* gameObject)
	{
		Robot* robot = (Robot*)gameObject;
	};

	Chase = [](void* gameObject)
	{
		Robot* robot = (Robot*)gameObject;
	};
}

void NCL::CSC8503::Robot::InitialiseStateTransitions()
{
	transitionDefaultToChase = new GenericTransition<bool&, bool>(GenericTransition <bool&, bool>::EqualsTransition,
		(bool&)playerInRange, true, robotStateDefault, robotStateChase);

	transitionChaseToDefault = new GenericTransition<bool&, bool>(GenericTransition <bool&, bool>::EqualsTransition,
		(bool&)playerInRange, false, robotStateChase, robotStateDefault);
}
