#include "GameState.h"

GameState::GameState(Level level)
{
	this->level = level;
	stateMachine = new StateMachine();

	InitialiseStates();
}

GameState::~GameState()
{
	delete stateMachine;
	stateMachine = NULL;
}

void GameState::InitialiseStates()
{
	if (!stateMachine)
		return;

	InitialiseStateFunctions();

	mainMenuState = new GenericState(MainMenu, this, "Menu");
	levelOneState = new GenericState(LevelOne, this, "Level 1");
	levelTwoState = new GenericState(LevelTwo, this, "Level 2");
	endSceneState = new GenericState(EndScene, this, "End Screen");

	stateMachine->AddState(mainMenuState);
	stateMachine->AddState(levelOneState);
	stateMachine->AddState(levelTwoState);
	stateMachine->AddState(endSceneState);

	InitialiseStateTransitions();

	stateMachine->AddTransition(transitionMenuToLevelOne);
	stateMachine->AddTransition(transitionMenuToLevelTwo);
	stateMachine->AddTransition(transitionLevelOneToLevelTwo);
	stateMachine->AddTransition(transitionLevelTwoToEndScene);
	stateMachine->AddTransition(transitionEndScreenToMenu);
	stateMachine->AddTransition(transitionLevelOneToMenu);
	stateMachine->AddTransition(transitionLevelTwoToMenu);
}

void GameState::InitialiseStateFunctions()
{
	MainMenu = [](void* state)
	{
		GameState* gameState = (GameState*)state;
	};

	LevelOne = [](void* state)
	{
		GameState* gameState = (GameState*)state;
	};

	LevelTwo = [](void* state)
	{
		GameState* gameState = (GameState*)state;
	};

	EndScene = [](void* state)
	{
		GameState* gameState = (GameState*)state;
	};
}

void GameState::InitialiseStateTransitions()
{
	transitionMenuToLevelOne = new GenericTransition<int&, int>(GenericTransition <int&, int>::EqualsTransition,
		(int&)level, LEVEL_1, mainMenuState, levelOneState);

	transitionMenuToLevelTwo = new GenericTransition<int&, int>(GenericTransition <int&, int>::EqualsTransition,
		(int&)level, LEVEL_2, mainMenuState, levelTwoState);

	transitionLevelOneToLevelTwo = new GenericTransition<int&, int>(GenericTransition <int&, int>::EqualsTransition,
		(int&)level, LEVEL_2, levelOneState, levelTwoState);

	transitionLevelTwoToEndScene = new GenericTransition<int&, int>(GenericTransition <int&, int>::EqualsTransition,
		(int&)level, END_SCENE, levelTwoState, endSceneState);

	transitionEndScreenToMenu = new GenericTransition<int&, int>(GenericTransition <int&, int>::EqualsTransition,
		(int&)level, MAIN_MENU, endSceneState, mainMenuState);

	transitionLevelOneToMenu = new GenericTransition<int&, int>(GenericTransition <int&, int>::EqualsTransition,
		(int&)level, MAIN_MENU, levelOneState, mainMenuState);

	transitionLevelTwoToMenu = new GenericTransition<int&, int>(GenericTransition <int&, int>::EqualsTransition,
		(int&)level, MAIN_MENU, levelTwoState, mainMenuState);
}
