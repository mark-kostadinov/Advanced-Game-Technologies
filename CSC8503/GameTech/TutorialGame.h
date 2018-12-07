#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "../CSC8503Common/definitions.h"
#include "../CSC8503Common/NavigationGrid.h"
#include "../CSC8503Common/GameState.h"
#include <iomanip>

namespace NCL {
	namespace CSC8503 {
		class TutorialGame		{
		public:
			TutorialGame();
			~TutorialGame();

			virtual void UpdateGame(float dt);
			void SetScreenSize(Vector2 screenSize) { this->screenSize = screenSize; }

		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			void ResetGame();
			void InitWorld();
			void LoadLevel(NavigationGrid* levelGrid);
			void DrawHitCounter(int hitCounter);

			/*
			These are some of the world/object creation functions I created when testing the functionality
			in the module. Feel free to mess around with them to see different objects being created in different
			test scenarios (constraints, collision types, and so on). 
			*/
			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);
			void InitSphereCollisionTorqueTest();
			void InitCubeCollisionTorqueTest();
			void InitSphereAABBTest();
			void InitGJKWorld();
			void BridgeConstraintTest();
			void SimpleGJKTest();
			void SimpleAABBTest();
			void SimpleAABBTest2();

			void UpdatePlayerObject();
			void UpdatePathfinding();
			void ResetScenes();

			GameObject* AddFloorToWorld(const Vector3& position, const Vector3& dimensions, string name = "Floor", Vector4 colour = Vector4(1, 1, 1, 1));
			GameObject* AddWallToWorld(const Vector3& position, const Vector3& dimensions, string name = "Wall", Vector4 colour = Vector4(1, 1, 1, 1));
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f, string name = "Sphere", 
				Vector4 colour = Vector4(1, 1, 1, 1), bool isHollow = NULL);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f, 
				string name = "Cube", Vector4 colour = Vector4(1, 1, 1, 1));
			Obstacle* AddObstacleToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 0.0f,
				string name = "Obstacle", Vector4 colour = Vector4(0.25f, 0.25f, 0.25f, 1));
			MovingObstacle* AddMovingObstacleToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 0.05f,
				string name = "Moving Obstacle", Vector4 colour = Vector4(1, 0, 0, 1));
			Goal* AddGoalToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 0.0f,
				string name = "Goal", Vector4 colour = Vector4(0.15f, 0.5f, 1, 1));
			Player* AddPlayerToWorld(const Vector3& position, float radius = 14.0f, float inverseMass = 200.0f, string name = "Player",
				Vector4 colour = Vector4(1, 1, 1, 1), bool isHollow = true);
			Robot* AddRobotToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 0.5f,
				string name = "Robot", Vector4 colour = Vector4(1, 1, 1, 1));
			Sand* AddSandToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 0.0f,
				string name = "Sand", Vector4 colour = Vector4(1, 1, 0.8f, 1));

			GameTechRenderer*	renderer;
			PhysicsSystem*		physics;
			GameWorld*			world;
			GameState*			gameState;
			vector<NavigationGrid*>	levelGrids;

			bool useGravity;
			bool renderMenu;
			bool renderEndScene;

			float forceMagnitude;
			Vector2	screenSize;
			Vector4 hitCounterColour;

			vector<MovingObstacle*>	movingObstacles;

			OGLMesh*	cubeMesh	= nullptr;
			OGLMesh*	sphereMesh	= nullptr;
			OGLTexture* basicTex	= nullptr;
			OGLTexture* golfBallTex	= nullptr;
			OGLTexture* grassTex	= nullptr;
			OGLTexture* sandTex		= nullptr;
			OGLTexture* robotTex	= nullptr;
			OGLTexture* wallTex		= nullptr;
			OGLShader*	basicShader = nullptr;
		};
	}
}

