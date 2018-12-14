#pragma once
#include "TutorialGame.h"
#include "../CSC8503Common/GameServer.h"
#include "../CSC8503Common/GameClient.h"
#include <algorithm>

namespace NCL
{
	namespace CSC8503
	{
		class ServerPacketReceiver;
		class PlayerPacketReceiver;

		class NetworkedGame : public TutorialGame
		{
		public:
			NetworkedGame();
			~NetworkedGame();

			void UpdateNetworking();

		protected:
			ServerPacketReceiver* serverStringReceiver;
			ServerPacketReceiver* serverGameDataReceiver;
			ServerPacketReceiver* serverConnectionReceiver;
			ServerPacketReceiver* serverDisconnectionReceiver;
			PlayerPacketReceiver* clientReceiver1;
			PlayerPacketReceiver* clientReceiver2;
			GameServer* server;
			GameClient* client1;
			GameClient* client2;
			int port;
			bool canConnect1;
			bool canConnect2;
		};

		class PlayerPacketReceiver : public PacketReceiver
		{
		public:
			PlayerPacketReceiver(string name) 
			{ 
				this->name = name; 
				playerID = 0;
				playerHits = 0;
				playerPosition = Vector3();
				playerOrientation = Quaternion();
			}

			void ReceivePacket(int type, GamePacket* payload, int source)
			{
				if (type == String)
				{
					StringPacket* realPacket = (StringPacket*)payload;
					string msg = realPacket->GetStringFromData();

					std::cout << name << " received message: " << msg << std::endl;
				}

				if (type == Game_Data)
				{
					GameDataPacket* realPacket = (GameDataPacket*)payload;
					playerID = realPacket->GetPlayerID();
					playerHits = realPacket->GetPlayerHits();
					playerPosition = realPacket->GetPlayerPosition();
					playerOrientation = realPacket->GetPlayerOrientation();
				}
			}

			int GetPlayerID() const { return playerID; }
			int GetPlayerHits() const { return playerHits; }
			Vector3 GetPlayerPosition() const { return playerPosition; }
			Quaternion GetPlayerOrientation() const { return playerOrientation; }

		protected:
			string name;
			int playerID;
			int playerHits;
			Vector3 playerPosition;
			Quaternion playerOrientation;
		};

		class ServerPacketReceiver : public PacketReceiver
		{
		public:
			ServerPacketReceiver(string name, GameServer* server) 
			{ 
				this->name = name; 
				this->server = server;
				playerID = 0;
				playerHits = 0;
			}

			void ReceivePacket(int type, GamePacket* payload, int source)
			{
				if (type == String)
				{
					StringPacket* realPacket = (StringPacket*)payload;
					string msg = realPacket->GetStringFromData();

					std::cout << name << " received message: " << msg << std::endl;
				}

				if (type == Player_Connected)
				{
					NewPlayerPacket* realPacket = (NewPlayerPacket*)payload;
					playerID = realPacket->GetPlayerID();
				}

				if (type == Player_Disconnected)
				{
					NewPlayerPacket* realPacket = (NewPlayerPacket*)payload;
					playerID = realPacket->GetPlayerID();
				}

				if (type == Game_Data)
				{
					GameDataPacket* realPacket = (GameDataPacket*)payload;
					playerID = realPacket->GetPlayerID();
					playerHits = realPacket->GetPlayerHits();

					//server->SendPacketToPeer(playerID, *realPacket);
					allScores.push_back(playerHits);
				}
			}

			int CalculateHighScore()
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

		protected:
			GameServer* server;
			string name;
			int playerID;
			int playerHits;
			vector<int> allScores;
		};
	};
};