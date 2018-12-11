#include "GameServer.h"
#include "GameWorld.h"
#include <iostream>

using namespace NCL;
using namespace CSC8503;

GameServer::GameServer(int onPort, int maxClients)
{
	port = onPort;
	clientMax = maxClients;
	clientCount = 0;
	netHandle = nullptr;
	threadAlive = false;
	highScore = 0;

	Initialise();
}

GameServer::~GameServer()
{
	Shutdown();
}

void GameServer::Shutdown()
{
	SendGlobalMessage(BasicNetworkMessages::Shutdown);

	threadAlive = false;
	updateThread.join();

	enet_host_destroy(netHandle);
	netHandle = nullptr;
}

bool GameServer::Initialise()
{
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = port;
	
	netHandle = enet_host_create(&address, clientMax, 1, 0, 0);
	
	if (!netHandle)
	{
		std::cout << __FUNCTION__ << " failed to create a network handle!" << std::endl;
		return false;
	}
	return true;
}

bool GameServer::SendGlobalMessage(int msgID)
{
	GamePacket packet;
	packet.type = msgID;
	return SendGlobalPacket(packet);
}

bool GameServer::SendGlobalPacket(GamePacket& packet)
{
	ENetPacket* dataPacket = enet_packet_create(&packet, packet.GetTotalSize(), 0);
	enet_host_broadcast(netHandle, 0, dataPacket);
	return true;
}

//bool GameServer::SendMessageToPeer(int peerID, int msgID)
//{
//	GamePacket packet;
//	packet.type = msgID;
//	return SendPacketToPeer(peerID, packet);
//}
//
//bool GameServer::SendPacketToPeer(int peerID, GamePacket& packet)
//{
//	if (currentConnections.empty())
//		return false;
//
//	ENetPacket* dataPacket = enet_packet_create(&packet, packet.GetTotalSize(), 0);
//
//	for (auto it = currentConnections.begin(); it != currentConnections.end(); it++)
//	{
//		if ((*it)->incomingPeerID == peerID)
//		{
//			enet_peer_send((*it), 0, dataPacket);
//			return true;
//		}
//	}
//	return false;
//}

void GameServer::UpdateServer()
{
	if (!netHandle)
		return;

	ENetEvent event;
	while (enet_host_service(netHandle, &event, 0) > 0)
	{
		int type = event.type;
		ENetPeer* p = event.peer;
		int peer = p->incomingPeerID;
		
		if (type == ENetEventType::ENET_EVENT_TYPE_CONNECT)
		{
			//currentConnections.push_back(p);
			std::cout << "Server: New client connected." << std::endl;
		}
		else if (type == ENetEventType::ENET_EVENT_TYPE_DISCONNECT)
		{
			std::cout << "Server: A client has disconnected." << std::endl;
			/*for (auto it = currentConnections.begin(); it != currentConnections.end(); it++)
			{
				if ((*it) == p)
					currentConnections.erase((*it));
			}*/
		}
		else if (type == ENetEventType::ENET_EVENT_TYPE_RECEIVE)
		{
			GamePacket* packet = (GamePacket*)event.packet->data;
			ProcessPacket(packet, peer);
		}
		enet_packet_destroy(event.packet);
	}
}

void GameServer::ThreadedUpdate()
{
	while (threadAlive)
	{
		UpdateServer();
		std::this_thread::yield();
	}
}

//Second networking tutorial stuff

void GameServer::SetGameWorld(GameWorld &g)
{
	gameWorld = &g;
}

void GameServer::BroadcastSnapshot(bool deltaFrame)
{

}

void GameServer::UpdateMinimumState()
{

}