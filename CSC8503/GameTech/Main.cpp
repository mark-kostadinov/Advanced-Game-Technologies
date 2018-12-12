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

class TestPacketReceiver : public PacketReceiver
{
public:
	TestPacketReceiver(string name) { this->name = name; }
	
	void ReceivePacket(int type, GamePacket* payload, int source)
	{
		if (type == String)
		{
			StringPacket* realPacket = (StringPacket*)payload;
			string msg = realPacket->GetStringFromData();
			
			std::cout << name << " received message: " << msg << std::endl;
		}
	}
protected:
	string name;
};

void TestNetworking()
{
	NetworkBase::Initialise();
	
	TestPacketReceiver serverReceiver("Server");
	TestPacketReceiver clientReceiver1("Client 1");
	TestPacketReceiver clientReceiver2("Client 2");
	
	int port = NetworkBase::GetDefaultPort();
	
	GameServer* server = new GameServer(port, 2);
	GameClient* client1 = new GameClient();
	GameClient* client2 = new GameClient();
	
	server->RegisterPacketHandler(String, &serverReceiver);
	client1->RegisterPacketHandler(String, &clientReceiver1);
	client2->RegisterPacketHandler(String, &clientReceiver2);
	
	bool canConnect1 = client1->Connect(127, 0, 0, 1, port);
	bool canConnect2 = client2->Connect(127, 0, 0, 1, port);
	
	if (canConnect1 && canConnect2)
	{
		for (int i = 0; i < 10; ++i)
		{
			server->SendGlobalPacket(StringPacket("Server says hello! " + std::to_string(i)));
			client1->SendPacket(StringPacket("Client 1 says hello! " + std::to_string(i)));
			client2->SendPacket(StringPacket("Client 2 says hello! " + std::to_string(i)));

			server->UpdateServer();
			client1->UpdateClient();
			client2->UpdateClient();

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
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
//////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
	Window* w = Window::CreateGameWindow("Totally original game of Golf!", 1200, 800, false);

	if (!w->HasInitialised())
		return -1;

	//TestStateMachine();
	//TestNetworking();
	//TestPathfinding();

	w->ShowOSPointer(true);
	w->LockMouseToWindow(false); // Edit the function to take in the parameter if you need a locked mouse -> it's bugged!

	NetworkedGame* g = new NetworkedGame();
	//TutorialGame* g = new TutorialGame();
	g->SetScreenSize(w->GetScreenSize());

	while (w->UpdateWindow() && !w->GetKeyboard()->KeyDown(KEYBOARD_ESCAPE))
	{
		float dt = w->GetTimer()->GetTimeDelta() / 1000.0f;

		if (dt > 1.0f)
			continue; // Must have hit a breakpoint or something to have a 1 second frame time

		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_PRIOR))
			w->ShowConsole(true);
		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_NEXT))
			w->ShowConsole(false);

		//DisplayPathfinding();

		g->UpdateNetworking();
		g->UpdateGame(dt);
	}
	Window::DestroyGameWindow();
}