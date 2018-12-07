#include "MovingObstacle.h"

MovingObstacle::MovingObstacle(string obstacleName, Direction dir)
{
	name = obstacleName;
	direction = dir;
	stateMachine = new StateMachine();

	InitialiseStates();
}

MovingObstacle::~MovingObstacle()
{
	delete stateMachine;
	stateMachine = NULL;
}

void MovingObstacle::InitialiseStates()
{
	if (!stateMachine)
		return;

	InitialiseStateFunctions();

	obstacleStateDefault = new GenericState(Default, this, "Default");
	obstacleStateLeft = new GenericState(GoLeft, this, "Left");
	obstacleStateRight = new GenericState(GoRight, this, "Right");
	obstacleStateUp = new GenericState(GoUp, this, "Up");
	obstacleStateDown = new GenericState(GoDown, this, "Down");

	stateMachine->AddState(obstacleStateDefault);
	stateMachine->AddState(obstacleStateLeft);
	stateMachine->AddState(obstacleStateRight);
	stateMachine->AddState(obstacleStateUp);
	stateMachine->AddState(obstacleStateDown);

	InitialiseStateTransitions();

	stateMachine->AddTransition(transitionDefaultToLeft);
	stateMachine->AddTransition(transitionLeftToRight);
	stateMachine->AddTransition(transitionRightToLeft);
}

void MovingObstacle::InitialiseStateFunctions()
{
	Default = [](void* gameObject)
	{
		MovingObstacle* obstacle = (MovingObstacle*)gameObject;
		obstacle->SetDirection(NONE);
	};

	GoLeft = [](void* gameObject)
	{
		MovingObstacle* obstacle = (MovingObstacle*)gameObject;
		obstacle->SetDirection(LEFT);
	};

	GoRight = [](void* gameObject)
	{
		MovingObstacle* obstacle = (MovingObstacle*)gameObject;
		obstacle->SetDirection(RIGHT);
	};

	GoUp = [](void* gameObject)
	{
		MovingObstacle* obstacle = (MovingObstacle*)gameObject;
		obstacle->SetDirection(UP);
	};

	GoDown = [](void* gameObject)
	{
		MovingObstacle* obstacle = (MovingObstacle*)gameObject;
		obstacle->SetDirection(DOWN);
	};
}

void MovingObstacle::InitialiseStateTransitions()
{
	// The obstacle starts moving to the Left first
	transitionDefaultToLeft = new GenericTransition<int, int>(GenericTransition <int, int>::EqualsTransition,
		direction, Direction::NONE, obstacleStateDefault, obstacleStateLeft);

	// If the value of the moving obstacle's position on the x axis is less than -200.0f, go Right
	transitionLeftToRight = new GenericTransition<GameObject*, float>(GenericTransition <GameObject*, float>::GameObjectLessThanTransition,
		this, 520.0f, obstacleStateLeft, obstacleStateRight);

	// If the value of the moving obstacle's position on the x axis is greater than 200.0f, go Left
	transitionRightToLeft = new GenericTransition<GameObject*, float>(GenericTransition <GameObject*, float>::GameObjectGreaterThanTransition,
		this, 1040.0f, obstacleStateRight, obstacleStateLeft);
}

void MovingObstacle::UpdateMovement()
{
	if (direction == LEFT)
		GetPhysicsObject()->ApplyLinearImpulse(Vector3(-200.0f, 0, 0));
	else if (direction == RIGHT)
		GetPhysicsObject()->ApplyLinearImpulse(Vector3(200.0f, 0, 0));
	else if (direction == UP)
		GetPhysicsObject()->ApplyLinearImpulse(Vector3(0, 0, 200.0f));
	else if (direction == DOWN)
		GetPhysicsObject()->ApplyLinearImpulse(Vector3(0, 0, -200.0f));
}
