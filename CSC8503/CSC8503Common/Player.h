#pragma once

#include "GameObject.h"

class Player : public NCL::CSC8503::GameObject
{
public:
	Player(string playerName = "Player");
	virtual ~Player();

	void OnCollisionBegin(GameObject* otherObject);
	void OnCollisionEnd(GameObject* otherObject);

	bool IsLevelCleared() const { return isLevelCleared; }
	void SetLevelCleared(bool b) { isLevelCleared = b; }

	int GetHitCounter() const { return hitCounter; }
	void SetHitCounter(int i) { hitCounter = i; }

	bool isLevelCleared;

protected:

	int hitCounter;
};
