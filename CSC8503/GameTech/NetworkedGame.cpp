#include "NetworkedGame.h"
#include "NetworkPlayer.h"

#define COLLISION_MSG 30

NetworkedGame::NetworkedGame()
{
	NetworkBase::Initialise();

	serverReceiver = new ServerPacketReceiver("Server");
	clientReceiver1 = new PlayerPacketReceiver("Client 1");
	clientReceiver2 = new PlayerPacketReceiver("Client 2");

	port = NetworkBase::GetDefaultPort();

	server = new GameServer(port, 2);
	client1 = new GameClient();
	client2 = new GameClient();

	server->RegisterPacketHandler(String, serverReceiver);
	client1->RegisterPacketHandler(String, clientReceiver1);
	client2->RegisterPacketHandler(String, clientReceiver2);

	canConnect1 = client1->Connect(127, 0, 0, 1, port);
	canConnect2 = client2->Connect(127, 0, 0, 1, port);
}

NetworkedGame::~NetworkedGame()
{
	NetworkBase::Destroy();
}

void NetworkedGame::UpdateNetworking()
{
	//for (auto it = server->GetCurrentConnections().begin(); it != server->GetCurrentConnections().end(); it++)
	//{
	//	if ((*it)->incomingPeerID == 2)
	//		server->SendPacketToPeer(2, StringPacket("We did it! " + std::to_string(i)));
	//}

	if (canConnect1)
	{
		vector<GameObject*>::const_iterator first;
		vector<GameObject*>::const_iterator last;
		GetGameWorld()->GetObjectIterators(first, last);

		for (auto it = first; it != last; ++it)
		{
			if ((*it)->GetName() == "Player")
				client1->SendPacket(StringPacket(std::to_string(((Player*)(*it))->GetHitCounter())));
		}
		client1->UpdateClient();
	}
	if (canConnect2)
	{
		client2->SendPacket(StringPacket(std::to_string(5)));
		client2->UpdateClient();
	}

	server->UpdateServer();
	serverReceiver->CalculateHighscore();
	server->SetHighScore(serverReceiver->GetHighScore());
	server->SendGlobalPacket(StringPacket("HS: " + std::to_string(server->GetHighScore())));
}

void NCL::CSC8503::ServerPacketReceiver::CalculateHighscore()
{
	/// TODO: Add some checks pls
	std::sort(allHighScores.at(0), allHighScores.at(1));

	highScore = allHighScores.at(0);
}
