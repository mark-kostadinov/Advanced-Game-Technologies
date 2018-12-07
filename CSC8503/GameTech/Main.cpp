#include "../../Common/Window.h"

#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/State.h"

#include "../CSC8503Common/GameServer.h"
#include "../CSC8503Common/GameClient.h"

#include "../CSC8503Common/NavigationGrid.h"

#include "TutorialGame.h"
#include "NetworkedGame.h"

using namespace NCL;
using namespace CSC8503;

void TestStateMachine()
{
	StateMachine* testMachine = new StateMachine();

	int someData = 0;

	StateFunc AFunc = [](void* data)
	{
		int* realData = (int*)data;
		(*realData)++;
		std::cout << "In State A!" << std::endl;
	};
	StateFunc BFunc = [](void * data)
	{
		int* realData = (int*)data;
		(*realData)--;
		std::cout << "In State B!" << std::endl;
	};

	GenericState* stateA = new GenericState(AFunc, (void*)& someData);
	GenericState* stateB = new GenericState(BFunc, (void*)& someData);
	testMachine->AddState(stateA);
	testMachine->AddState(stateB);

	GenericTransition<int&, int>* transitionA = new GenericTransition<int&, int>(
		GenericTransition <int &, int>::GreaterThanTransition, someData, 10, stateA, stateB); // If greater than 10, A to B

	GenericTransition<int &, int>* transitionB = new GenericTransition<int &, int>(
		GenericTransition <int &, int >::EqualsTransition, someData, 0, stateB, stateA); // If equals 0, B to A

	testMachine->AddTransition(transitionA);
	testMachine->AddTransition(transitionB);

	for (int i = 0; i < 100; ++i)
		testMachine->Update(); // Run the state machine

	delete testMachine;
}

void TestNetworking()
{

}

vector<Vector3> testNodes;

void TestPathfinding()
{
	NavigationGrid grid("TestGrid.txt");
	
	NavigationPath outPath;
	
	Vector3 startPos(800, 0, 100);
	Vector3 endPos(800, 0, 800);
	
	bool found = grid.FindPath(startPos, endPos, outPath);
	
	Vector3 pos;
	while (outPath.PopWaypoint(pos))
		testNodes.push_back(pos);
}

void DisplayPathfinding()
{
	for (int i = 1; i < (int)testNodes.size(); ++i)
	{
		Vector3 a = testNodes[i - 1];
		Vector3 b = testNodes[i];
		
		Debug::DrawLine(a, b, Vector4(0, 1, 1, 1));
	}
}

int main()
{
	Window* w = Window::CreateGameWindow("Totally original Golf game!", 1920, 1200, true);

	if (!w->HasInitialised())
		return -1;

	//TestStateMachine();
	//TestNetworking();
	//TestPathfinding();

	w->ShowOSPointer(true);
	w->LockMouseToWindow(true);

	TutorialGame* g = new TutorialGame();
	g->SetScreenSize(w->GetScreenSize());

	while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE))
	{
		float dt = w->GetTimer()->GetTimeDelta() / 1000.0f;

		if (dt > 1.0f)
			continue; // Must have hit a breakpoint or something to have a 1 second frame time

		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_PRIOR))
			w->ShowConsole(true);
		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_NEXT))
			w->ShowConsole(false);

		//DisplayPathfinding();

		g->UpdateGame(dt);
	}
	Window::DestroyGameWindow();
}