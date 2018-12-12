#include "NetworkedGame.h"
#include "NetworkPlayer.h"

#define COLLISION_MSG 30

NetworkedGame::NetworkedGame()
{
	NetworkBase::Initialise();
again:
	std::cout << "Are you the first player? (y/n)" << std::endl;
	char ans;
	std::cin >> ans;

	if (ans == 'y')
	{
		serverStringReceiver = new ServerPacketReceiver("Server String Receiver");
		serverGameDataReceiver = new ServerPacketReceiver("Server Game Data Receiver");
		serverConnectionReceiver = new ServerPacketReceiver("Server Connection Receiver");
		serverDisconnectionReceiver = new ServerPacketReceiver("Server Disconnection Receiver");
		clientReceiver1 = new PlayerPacketReceiver("Client " + std::to_string(currentPlayerID));

		port = NetworkBase::GetDefaultPort();
		server = new GameServer(port, 2);
		client1 = new GameClient();

		server->RegisterPacketHandler(String, serverStringReceiver);
		server->RegisterPacketHandler(Game_Data, serverGameDataReceiver);
		server->RegisterPacketHandler(Player_Connected, serverConnectionReceiver);
		server->RegisterPacketHandler(Player_Disconnected, serverDisconnectionReceiver);

		client1->RegisterPacketHandler(String, clientReceiver1);
		canConnect1 = client1->Connect(127, 0, 0, 1, port);
		client1->SendPacket(NewPlayerPacket(currentPlayerID));
	}
	else if (ans == 'n')
	{
		std::cout << "Enter a player ID: " << std::endl;
		std::cin >> currentPlayerID;

		clientReceiver2 = new PlayerPacketReceiver("Client " + std::to_string(currentPlayerID));
		client2 = new GameClient();
		client2->RegisterPacketHandler(String, clientReceiver2);
		canConnect2 = client2->Connect(127, 0, 0, 1, port);
		client2->SendPacket(NewPlayerPacket(currentPlayerID));
	}
	else
		goto again;
}

NetworkedGame::~NetworkedGame()
{
	NetworkBase::Destroy();
}

void NetworkedGame::UpdateNetworking()
{
	vector<GameObject*>::const_iterator first;
	vector<GameObject*>::const_iterator last;
	GetGameWorld()->GetObjectIterators(first, last);
	vector<Player*> allPlayers;

	for (auto it = first; it != last; ++it) // Get references to all of the players
	{
		if ((*it)->GetName() == "Player")
			allPlayers.push_back((Player*)(*it));
	}

	if (canConnect1)
	{
		if (!allPlayers.empty())
		{
			int playerID = allPlayers.at(0)->GetPlayerID(); /// TODO
			int playerHits = allPlayers.at(0)->GetHitCounter();
			Vector3 playerPosition = allPlayers.at(0)->GetTransform().GetWorldPosition();
			Quaternion playerOrientation = allPlayers.at(0)->GetTransform().GetWorldOrientation();

			client1->SendPacket(GameDataPacket(playerID, playerHits, playerPosition, playerOrientation));
		}
		client1->UpdateClient();
	}
	if (canConnect2)
	{
		/// TODO: Set the values to the opposite player from the server inside this function
		//client2->SendPacket(GameDataPacket(2, 5, Vector3(), Quaternion()));

		if (!allPlayers.empty())
		{
			int playerID = allPlayers.at(1)->GetPlayerID();
			int playerHits = allPlayers.at(1)->GetHitCounter();
			Vector3 playerPosition = allPlayers.at(1)->GetTransform().GetWorldPosition();
			Quaternion playerOrientation = allPlayers.at(1)->GetTransform().GetWorldOrientation();

			client2->SendPacket(GameDataPacket(playerID, playerHits, playerPosition, playerOrientation));
		}
		client2->UpdateClient();
	}

	if (server)
	{
		server->UpdateServer();
		server->SetHighScore(serverGameDataReceiver->CalculateHighScore());
		server->SendGlobalPacket(StringPacket("High Score: " + std::to_string(server->GetHighScore())));
	}
}

int ServerPacketReceiver::CalculateHighScore()
{
	int hs = 0;

	if (!allScores.empty())
	{
		std::sort(allScores.begin(), allScores.end(), [](int const& a, int const& b) { return a < b; });
		hs = allScores.at(0);
		allScores.clear();
	}
	return hs;
}
