#pragma once

#include "GameObject.h"

class Obstacle : public NCL::CSC8503::GameObject
{
public:
	Obstacle(string obstacleName = "Obstacle");
	~Obstacle();
};

