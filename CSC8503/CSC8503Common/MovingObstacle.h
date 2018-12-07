#pragma once

#include "Obstacle.h"

#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/State.h"

using namespace NCL;
using namespace CSC8503;

enum Direction
{
	NONE, LEFT, RIGHT, UP, DOWN
};

class MovingObstacle : public Obstacle
{
public:
	MovingObstacle(string obstacleName = "Moving Obstacle", Direction dir = NONE);
	~MovingObstacle();

	void InitialiseStates();
	void InitialiseStateFunctions();
	void InitialiseStateTransitions();
	void UpdateMovement();

	Direction GetDirection() const { return direction; }
	void SetDirection(Direction d) { direction = d; }

	StateMachine* GetStateMachine() const { return stateMachine; }

protected:
	Direction direction;

	StateMachine* stateMachine;

	GenericState* obstacleStateDefault;
	GenericState* obstacleStateLeft;
	GenericState* obstacleStateRight;
	GenericState* obstacleStateUp;
	GenericState* obstacleStateDown;

	StateFunc Default;
	StateFunc GoLeft;
	StateFunc GoRight;
	StateFunc GoUp;
	StateFunc GoDown;

	GenericTransition<int, int>* transitionDefaultToLeft;
	GenericTransition<GameObject*, float>* transitionLeftToRight;
	GenericTransition<GameObject*, float>* transitionRightToLeft;
};

