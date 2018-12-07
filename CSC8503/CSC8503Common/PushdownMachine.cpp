#include "PushdownMachine.h"
#include "PushdownState.h"
using namespace NCL::CSC8503;

PushdownMachine::PushdownMachine()
{
	activeState = nullptr;
}

PushdownMachine::~PushdownMachine()
{
}

void PushdownMachine::Update() 
{
	if (activeState)
		activeState->Update();
}

void PushdownMachine::PushState(PushdownState* ps)
{
	stateStack.push(ps);
	activeState = stateStack.top();
}

void PushdownMachine::PopState()
{
	stateStack.pop();
	activeState = stateStack.top();
}
