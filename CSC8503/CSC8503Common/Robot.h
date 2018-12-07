#pragma once

#include "GameObject.h"

class Robot : public NCL::CSC8503::GameObject
{
public:
	Robot(string name = "Robot");
	~Robot();

	void OnCollisionBegin(GameObject* otherObject);
	void OnCollisionEnd(GameObject* otherObject);
};
