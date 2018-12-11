#pragma once
#include <thread>
#include <atomic>
#include <vector>
#include <iterator>

#include "NetworkBase.h"

namespace NCL
{
	namespace CSC8503
	{
		class GameWorld;
		class GameServer : public NetworkBase
		{
		public:
			GameServer(int onPort, int maxClients);
			~GameServer();

			bool Initialise();
			void Shutdown();

			virtual void UpdateServer();

			void SetGameWorld(GameWorld &g);

			void ThreadedUpdate();

			bool SendGlobalMessage(int msgID);
			bool SendGlobalPacket(GamePacket& packet);
			//bool SendMessageToPeer(int peerID, int msgID);
			//bool SendPacketToPeer(int peerID, GamePacket& packet);

			//vector<ENetPeer*> GetCurrentConnections() const { return currentConnections; }
			int GetHighScore() const { return highScore; }
			void SetHighScore(int i) { highScore = i; }

			void BroadcastSnapshot(bool deltaFrame);
			void UpdateMinimumState();

		protected:
			int			port;
			int			clientMax;
			int			clientCount;
			GameWorld*	gameWorld;

			//vector<ENetPeer*> currentConnections;
			int highScore;

			std::atomic<bool> threadAlive;
			std::thread updateThread;

			int incomingDataRate;
			int outgoingDataRate;

			std::map<int, int> stateIDs;
		};
	}
}
