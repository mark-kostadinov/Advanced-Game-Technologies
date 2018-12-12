#include "Player.h"

Player::Player(int playerID, string playerName)
{
	id = playerID;
	name = playerName;
	isAlive = true;
	isLevelCleared = false;
	hitCounter = 0;
}

Player::~Player()
{
}

void Player::OnCollisionBegin(GameObject * otherObject)
{
	if (otherObject->GetName() == "Floor")
		GetPhysicsObject()->SetFriction(0.3f);
	if (otherObject->GetName() == "Sand")
		GetPhysicsObject()->SetFriction(0.001f);
}

void Player::OnCollisionEnd(GameObject * otherObject)
{
	// If the player is colliding with a moving obstacle => kill them
	if (otherObject->GetName() == "Moving Obstacle")
		Kill();
	// If the player is colliding with a robot => kill them
	if (otherObject->GetName() == "Robot")
		Kill();
	// If the player has reached the goal, load the next level (if there is one)
	if (otherObject->GetName() == "Goal")
		isLevelCleared = true;
	// Reset the friction of the "air" to a very small value?
	GetPhysicsObject()->SetFriction(0.95f);
}
