#include "PushdownState.h"

using namespace NCL::CSC8503;

PushdownState::PushdownState(std::string name)
{
	this->name = name;
}

PushdownState::~PushdownState()
{
}

PushdownState::PushdownResult PushdownState::PushdownUpdate(PushdownState* pushResult) 
{
	return PushdownResult::NoChange;
}