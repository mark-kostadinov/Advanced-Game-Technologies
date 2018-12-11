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
			ServerPacketReceiver* serverReceiver;
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
			PlayerPacketReceiver(string name) { this->name = name; }

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

		class ServerPacketReceiver : public PacketReceiver
		{
		public:
			ServerPacketReceiver(string name) { this->name = name; }

			void ReceivePacket(int type, GamePacket* payload, int source)
			{
				if (type == String)
				{
					StringPacket* realPacket = (StringPacket*)payload;
					string msg = realPacket->GetStringFromData();

					std::cout << name << " received message: " << msg << std::endl;

					/// TODO: Make a Highscore packet type
					allHighScores.push_back(std::stoi(msg));
				}
			}

			void CalculateHighscore();
			int GetHighScore() const { return highScore; }

		protected:
			string name;

			vector<int> allHighScores;
			int highScore;
		};
	};
};