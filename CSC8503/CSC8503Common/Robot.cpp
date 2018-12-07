#include "Robot.h"

Robot::Robot(string name)
{
	this->name = name;
}

Robot::~Robot()
{
}

void Robot::OnCollisionBegin(GameObject * otherObject)
{
	if (otherObject->GetName() == "Floor")
		GetPhysicsObject()->SetFriction(0.5f);
	if (otherObject->GetName() == "Sand")
		GetPhysicsObject()->SetFriction(0.001f);
}

void Robot::OnCollisionEnd(GameObject * otherObject)
{
	GetPhysicsObject()->SetFriction(0.95f);
}
