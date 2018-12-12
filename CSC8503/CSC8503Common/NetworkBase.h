#pragma once
#include <enet/enet.h>
#include <map>
#include <string>
#include <iostream>

#include "../../Common/Vector3.h"
#include "../../Common/Quaternion.h"

using namespace NCL;
using namespace NCL::Maths;

enum BasicNetworkMessages
{
	None,
	Hello,
	Message,
	String,
	Game_Data,
	Delta_State,	//1 byte per channel since the last state
	Full_State,		//Full transform etc
	Received_State, //received from a client, informs that its received packet n
	Player_Connected,
	Player_Disconnected,
	Shutdown
};

struct GamePacket
{
	short size;
	short type;

	GamePacket()
	{
		type = BasicNetworkMessages::None;
		size = 0;
	}

	GamePacket(short type) { this->type = type; }

	int GetTotalSize() { return sizeof(GamePacket) + size; }
};

struct StringPacket : public GamePacket
{
	char stringData[256];

	StringPacket(const std::string& message)
	{
		type = BasicNetworkMessages::String;
		size = (short)message.length();

		memcpy(stringData, message.data(), size);
	};

	std::string GetStringFromData()
	{
		std::string realString(stringData);
		realString.resize(size);
		return realString;
	}
};

struct GameDataPacket : public GamePacket
{
	int playerID;
	int playerHits;
	Vector3 playerPosition;
	Quaternion playerOrientation;

	GameDataPacket(const int id, int hits, const Vector3& position, const Quaternion& orientation)
	{
		type = BasicNetworkMessages::Game_Data;
		playerID = id;
		playerHits = hits;
		playerPosition = position;
		playerOrientation = orientation;
		size = sizeof(int) + sizeof(int) + sizeof(Vector3) + sizeof(Quaternion);
	};

	int GetPlayerID() const { return playerID; }
	int GetPlayerHits() const { return playerHits; }
	Vector3 GetPlayerPosition() const { return playerPosition; }
	Quaternion GetPlayerOrientation() const { return playerOrientation; }
};

struct NewPlayerPacket : public GamePacket
{
	int playerID;

	NewPlayerPacket(int p)
	{
		type = BasicNetworkMessages::Player_Connected;
		playerID = p;
		size = sizeof(int);
	}

	int GetPlayerID() const { return playerID; }
};

struct PlayerDisconnectPacket : public GamePacket
{
	int playerID;

	PlayerDisconnectPacket(int p)
	{
		type = BasicNetworkMessages::Player_Disconnected;
		playerID = p;
		size = sizeof(int);
	}

	int GetPlayerID() const { return playerID; }
};

class PacketReceiver
{
public:
	virtual void ReceivePacket(int type, GamePacket* payload, int source = -1) = 0;
};

class NetworkBase
{
public:
	static void Initialise();
	static void Destroy();

	static int GetDefaultPort() { return 1234; }

	void RegisterPacketHandler(int msgID, PacketReceiver* receiver) { packetHandlers.insert(std::make_pair(msgID, receiver)); }

protected:
	NetworkBase();
	~NetworkBase();

	bool ProcessPacket(GamePacket * p, int peerID = -1);

	typedef std::multimap<int, PacketReceiver*>::const_iterator PacketHandlerIterator;

	bool GetPacketHandlers(int msgID, PacketHandlerIterator& first, PacketHandlerIterator& last) const
	{
		auto range = packetHandlers.equal_range(msgID);

		if (range.first == packetHandlers.end())
		{
			return false; //no handlers for this message type!
		}
		first = range.first;
		last = range.second;
		return true;
	}

	ENetHost* netHandle;
	std::multimap<int, PacketReceiver*> packetHandlers;
};