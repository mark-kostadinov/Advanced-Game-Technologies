#pragma once

#include "State.h"
#include "StateMachine.h"
#include "StateTransition.h"
#include "GameObject.h"

using namespace NCL;
using namespace CSC8503;

enum Level
{
	MAIN_MENU, LEVEL_1, LEVEL_2, END_SCENE
};

class GameState
{
public:
	GameState(Level level = MAIN_MENU);
	~GameState();

	void InitialiseStates();
	void InitialiseStateFunctions();
	void InitialiseStateTransitions();

	Level GetLevel() const { return level; }
	void SetLevel(Level lvl) { level = lvl; }

	StateMachine* GetStateMachine() const { return stateMachine; }

protected:
	Level level;

	StateMachine* stateMachine;

	GenericState* mainMenuState;
	GenericState* levelOneState;
	GenericState* levelTwoState;
	GenericState* endSceneState;

	StateFunc MainMenu;
	StateFunc LevelOne;
	StateFunc LevelTwo;
	StateFunc EndScene;

	GenericTransition<int&, int>* transitionMenuToLevelOne;
	GenericTransition<int&, int>* transitionMenuToLevelTwo;
	GenericTransition<int&, int>* transitionLevelOneToLevelTwo;
	GenericTransition<int&, int>* transitionLevelTwoToEndScene;
	GenericTransition<int&, int>* transitionEndScreenToMenu;
	GenericTransition<int&, int>* transitionLevelOneToMenu;
	GenericTransition<int&, int>* transitionLevelTwoToMenu;
};