#include "TutorialGame.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"

#include "../CSC8503Common/PositionConstraint.h"

using namespace NCL;
using namespace CSC8503;

bool renderQuadTreeMode = false;

TutorialGame::TutorialGame()
{
	world = new GameWorld();
	renderer = new GameTechRenderer(*world);
	physics = new PhysicsSystem(*world);
	gameState = new GameState(MAIN_MENU);

	forceMagnitude = 50.0f;
#ifdef DEBUG_MODE
	useGravity = false;
#endif // DEBUG_MODE
#ifndef DEBUG_MODE
	useGravity = true;
#endif DEBU_MODE
	physics->UseGravity(useGravity);

	renderMenu = false;
	renderEndScene = false;
	isPlayerInRange = false;
	drawPathfindingLines = false;
	currentPlayerID = 1;

	Debug::SetRenderer(renderer);

	InitialiseAssets();

	Window::GetWindow()->ShowOSPointer(true);
	Window::GetWindow()->LockMouseToWindow(true);
}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes,
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it

*/
void TutorialGame::InitialiseAssets()
{
	cubeMesh = new OGLMesh("cube.msh");
	cubeMesh->SetPrimitiveType(GeometryPrimitive::Triangles);
	cubeMesh->UploadToGPU();

	sphereMesh = new OGLMesh("sphere.msh");
	sphereMesh->SetPrimitiveType(GeometryPrimitive::Triangles);
	sphereMesh->UploadToGPU();

	basicTex = (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	golfBallTex = (OGLTexture*)TextureLoader::LoadAPITexture("golfBall.png");
	renderer->EnableAnisotropicFiltering(*golfBallTex, renderer->largestSupportedAnisotropyLevel);
	grassTex = (OGLTexture*)TextureLoader::LoadAPITexture("grass.png");
	renderer->EnableAnisotropicFiltering(*grassTex, renderer->largestSupportedAnisotropyLevel);
	sandTex = (OGLTexture*)TextureLoader::LoadAPITexture("sand.jpg");
	renderer->EnableAnisotropicFiltering(*sandTex, renderer->largestSupportedAnisotropyLevel);
	robotTex = (OGLTexture*)TextureLoader::LoadAPITexture("nyan.png");
	renderer->EnableAnisotropicFiltering(*robotTex, renderer->largestSupportedAnisotropyLevel);
	wallTex = (OGLTexture*)TextureLoader::LoadAPITexture("wood.jpg");
	renderer->EnableAnisotropicFiltering(*wallTex, renderer->largestSupportedAnisotropyLevel);
	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");

	levelGrids.push_back(new NavigationGrid("Level_1_Grid.txt"));
	levelGrids.push_back(new NavigationGrid("Level_2_Grid.txt"));

	InitCamera();
	InitWorld();
}

TutorialGame::~TutorialGame()
{
	delete cubeMesh;
	cubeMesh = NULL;
	delete sphereMesh;
	sphereMesh = NULL;
	delete basicTex;
	basicTex = NULL;
	delete golfBallTex;
	golfBallTex = NULL;
	delete grassTex;
	grassTex = NULL;
	delete sandTex;
	sandTex = NULL;
	delete robotTex;
	robotTex = NULL;
	delete wallTex;
	wallTex = NULL;
	delete basicShader;
	basicShader = NULL;

	delete physics;
	physics = NULL;
	delete renderer;
	renderer = NULL;
	delete world;
	world = NULL;
	delete gameState;
	gameState = NULL;
}

void TutorialGame::UpdateGame(float dt)
{
	world->GetMainCamera()->UpdateCamera(dt);
	gameState->GetStateMachine()->Update();

	UpdateKeys();

	if (renderMenu)
	{
		renderer->DrawString("Press 1 for Level 1", Vector2(screenSize.x / 2.0f - 110.0f, screenSize.y / 2.0f));
		renderer->DrawString("Press 2 for Level 2", Vector2(screenSize.x / 2.0f - 110.0f, screenSize.y / 2.0f - 40.0f));
	}
	if (renderEndScene)
	{
		renderer->DrawString("Congratulations, you won!", Vector2(screenSize.x / 2.0f - 170.0f, screenSize.y / 2.0f));
		renderer->DrawString("Press M to go back to the Menu", Vector2(screenSize.x / 2.0f - 200.0f, screenSize.y / 2.0f - 40.0f));
	}
	else
	{
#ifdef DEBUG_MODE
		if (useGravity)
			Debug::Print("(G)ravity on", Vector2(10, 35));
		else
			Debug::Print("(G)ravity off", Vector2(10, 35));
#endif //DEBUG_MODE

		UpdatePlayer();
		UpdateRobot();

		if (!movingObstacles.empty())
		{
			for (auto it = movingObstacles.begin(); it != movingObstacles.end(); it++)
			{
				(*it)->GetStateMachine()->Update();
				(*it)->UpdateMovement();
			}
		}
	}
	// Draw stuff on screen
	renderer->DrawString(std::to_string((int)std::trunc(1000.0f * dt)) + " ms", Vector2(screenSize.x - 100.0f, screenSize.y - 35.0f),
		Vector4(0.4, 1, 0, 1));
	Vector2 titlePosition = Vector2(screenSize.x / 2.0f - 20.0f, screenSize.y - 40.0f);
	if (renderEndScene)
		titlePosition = Vector2(screenSize.x / 2.0f - 40.0f, screenSize.y / 2.0f + 70.0f);

	renderer->DrawString(gameState->GetStateMachine()->GetActiveState()->GetName(), titlePosition, Vector4(1.0f, 1.0f, 1.0f, 1.0f));

	world->UpdateWorld(dt);
	renderer->Update(dt);
	physics->Update(dt);

	Debug::FlushRenderables();
	renderer->Render();
}

void TutorialGame::UpdateKeys()
{
	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_M))
	{
		gameState->SetLevel(MAIN_MENU);
		ResetGame();
	}
	if (gameState->GetLevel() == MAIN_MENU && Window::GetKeyboard()->KeyPressed(KEYBOARD_1))
	{
		gameState->SetLevel(LEVEL_1);
		ResetGame();
	}
	if (gameState->GetLevel() == MAIN_MENU && Window::GetKeyboard()->KeyPressed(KEYBOARD_2))
	{
		gameState->SetLevel(LEVEL_2);
		ResetGame();
	}

	// Some commands for the spec and more!
	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_R))
		ResetGame();
	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_T))
		renderQuadTreeMode = !renderQuadTreeMode;
	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_P))
		drawPathfindingLines = !drawPathfindingLines;

#ifdef DEBUG_MODE
	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::KEYBOARD_G))
	{
		useGravity = !useGravity; // Toggle gravity!
		physics->UseGravity(useGravity);
	}

	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F9))
		world->ShuffleConstraints(true);

	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F10))
		world->ShuffleConstraints(false);

	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F7))
		world->ShuffleObjects(true);

	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F8))
		world->ShuffleObjects(false);
#endif // DEBUG_MODE
}

void TutorialGame::InitCamera()
{
	world->GetMainCamera()->SetNearPlane(1.0f);
	world->GetMainCamera()->SetFarPlane(5000.0f);
	world->GetMainCamera()->SetPitch(-30.0f);
	world->GetMainCamera()->SetYaw(0.0f);
	world->GetMainCamera()->SetPosition(Vector3(750, 250, 2000));
}

void TutorialGame::ResetGame()
{
	ResetScenes();
	InitWorld(); // Reset the simulation
	InitCamera(); // Reset the camera to a specific default place
}

void TutorialGame::InitWorld()
{
	movingObstacles.clear();
	world->ClearAndErase();
	physics->Clear();

	if (gameState->GetLevel() == MAIN_MENU)
		renderMenu = true;
	else if (gameState->GetLevel() == END_SCENE)
		renderEndScene = true;
	else
		LoadLevel(levelGrids.at(gameState->GetLevel() - 1));

	//InitCubeGridWorld(5, 5, 50.0f, 50.0f, Vector3(10, 10, 10));
	//InitSphereGridWorld(2, 2, 50.0f, 50.0f, 10.0f);
	//InitSphereCollisionTorqueTest(w);
	//InitCubeCollisionTorqueTest(w);

	//InitSphereGridWorld(w, 1, 1, 50.0f, 50.0f, 10.0f);
	//BridgeConstraintTest(w);
	//InitGJKWorld(w);

	//DodgyRaycastTest(w);
	//InitGJKWorld(w);
	//InitSphereAABBTest(w);
	//SimpleGJKTest(w);
	//SimpleAABBTest2(w);

	//InitSphereCollisionTorqueTest(w);
}

void TutorialGame::LoadLevel(NavigationGrid* levelGrid)
{
	AddFloorToWorld(Vector3(780, -600, 780), Vector3(800, 500, 1000));
	AddWallToWorld(Vector3(780, -90, -240), Vector3(800, 10, 20));
	AddWallToWorld(Vector3(780, -90, 1800), Vector3(800, 10, 20));
	AddWallToWorld(Vector3(-40, -90, 780), Vector3(20, 10, 1040));
	AddWallToWorld(Vector3(1600, -90, 780), Vector3(20, 10, 1040));
	int numPlayers = 0;

	for (int i = 0; i < levelGrid->GetGridSize(); i++)
	{
		GridNode* currentGridNode = levelGrid->GetNodeAt(i);

		switch (currentGridNode->type)
		{
		case GridNode::OBSTACLE_NODE:
			AddObstacleToWorld(Vector3(currentGridNode->position.x, -40.0f, currentGridNode->position.z),
				Vector3((float)levelGrid->GetNodeSize(), levelGrid->GetNodeSize() * 3.0f, (float)levelGrid->GetNodeSize()));
			break;
		case GridNode::PLAYER_NODE:
			numPlayers++;
			AddPlayerToWorld(numPlayers, Vector3(currentGridNode->position.x, 0.0f, currentGridNode->position.z));
			break;
		case GridNode::MOVING_OBSTACLE_NODE:
			AddMovingObstacleToWorld(Vector3(currentGridNode->position.x, 0.0f, currentGridNode->position.z), Vector3(50.0f, 50.0f, 50.0f));
			break;
		case GridNode::GOAL_NODE:
			AddGoalToWorld(Vector3(currentGridNode->position.x, -99.0f, currentGridNode->position.z),
				Vector3(levelGrid->GetNodeSize() * 2.0f, 0.5f, levelGrid->GetNodeSize() * 2.0f));
			break;
		case GridNode::ROBOT_NODE:
			AddRobotToWorld(Vector3(currentGridNode->position.x, -30.0f, currentGridNode->position.z), Vector3(20.0f, 50.0f, 20.0f));
			break;
		case GridNode::SAND_NODE:
			AddSandToWorld(Vector3(currentGridNode->position.x, -100.0f, currentGridNode->position.z),
				Vector3(levelGrid->GetNodeSize() * 7.0f, 0.5f, levelGrid->GetNodeSize() * 7.0f));
			break;
		}
	}
}

void TutorialGame::DrawHitCounter(int hitCounter)
{
	switch (hitCounter)
	{
	case 0:
		hitCounterColour = Vector4(0.25f, 1, 0, 1);
		break;
	case 5:
		hitCounterColour = Vector4(0.65f, 1, 0, 1);
		break;
	case 10:
		hitCounterColour = Vector4(1, 1, 0, 1);
		break;
	case 15:
		hitCounterColour = Vector4(1, 0.65f, 0, 1);
		break;
	case 20:
		hitCounterColour = Vector4(1, 0.25f, 0, 1);
		break;
	case 25:
		hitCounterColour = Vector4(1, 0, 0, 1);
		break;
	}
	renderer->DrawString("Hit counter: " + std::to_string(hitCounter), Vector2(10.0f, screenSize.y - 35.0f), hitCounterColour);
}

GameObject* TutorialGame::AddFloorToWorld(const Vector3& position, const Vector3& dimensions, string name, Vector4 colour)
{
	GameObject* floor = new GameObject(name);

	Vector3 floorSize = dimensions;
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform().SetWorldScale(floorSize);
	floor->GetTransform().SetWorldPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, grassTex, basicShader));
	floor->GetRenderObject()->SetColour(colour);
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(floor);

	return floor;
}

GameObject* TutorialGame::AddWallToWorld(const Vector3 & position, const Vector3& dimensions, string name, Vector4 colour)
{
	GameObject* wall = new GameObject(name);

	Vector3 wallSize = dimensions;
	AABBVolume* volume = new AABBVolume(wallSize);
	wall->SetBoundingVolume((CollisionVolume*)volume);
	wall->GetTransform().SetWorldScale(wallSize);
	wall->GetTransform().SetWorldPosition(position);

	wall->SetRenderObject(new RenderObject(&wall->GetTransform(), cubeMesh, wallTex, basicShader));
	wall->GetRenderObject()->SetColour(colour);
	wall->SetPhysicsObject(new PhysicsObject(&wall->GetTransform(), wall->GetBoundingVolume()));

	wall->GetPhysicsObject()->SetInverseMass(0);
	wall->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(wall);

	return wall;
}

GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass, string name, Vector4 colour, bool isHollow)
{
	GameObject* sphere = new GameObject(name);

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);
	sphere->GetTransform().SetWorldScale(sphereSize);
	sphere->GetTransform().SetWorldPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->GetRenderObject()->SetColour(colour);
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);

	if (isHollow)
		sphere->GetPhysicsObject()->InitSphereInertia(true);
	else
		sphere->GetPhysicsObject()->InitSphereInertia(false);

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass, string name, Vector4 colour)
{
	GameObject* cube = new GameObject(name);

	AABBVolume* volume = new AABBVolume(dimensions);

	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform().SetWorldPosition(position);
	cube->GetTransform().SetWorldScale(dimensions);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->GetRenderObject()->SetColour(colour);
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}

Obstacle* TutorialGame::AddObstacleToWorld(const Vector3 & position, Vector3 dimensions, float inverseMass, string name, Vector4 colour)
{
	Obstacle* obstacleObject = new Obstacle();

	AABBVolume* volume = new AABBVolume(dimensions);

	obstacleObject->SetBoundingVolume((CollisionVolume*)volume);

	obstacleObject->GetTransform().SetWorldPosition(position);
	obstacleObject->GetTransform().SetWorldScale(dimensions);

	obstacleObject->SetRenderObject(new RenderObject(&obstacleObject->GetTransform(), cubeMesh, basicTex, basicShader));
	obstacleObject->GetRenderObject()->SetColour(colour);
	obstacleObject->SetPhysicsObject(new PhysicsObject(&obstacleObject->GetTransform(), obstacleObject->GetBoundingVolume()));

	obstacleObject->GetPhysicsObject()->SetInverseMass(inverseMass);
	obstacleObject->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(obstacleObject);

	return obstacleObject;
}

MovingObstacle* TutorialGame::AddMovingObstacleToWorld(const Vector3 & position, Vector3 dimensions, float inverseMass, string name, Vector4 colour)
{
	MovingObstacle* movingObstacleObject = new MovingObstacle();

	AABBVolume* volume = new AABBVolume(dimensions);

	movingObstacleObject->SetBoundingVolume((CollisionVolume*)volume);

	movingObstacleObject->GetTransform().SetWorldPosition(position);
	movingObstacleObject->GetTransform().SetWorldScale(dimensions);

	movingObstacleObject->SetRenderObject(new RenderObject(&movingObstacleObject->GetTransform(), cubeMesh, basicTex, basicShader));
	movingObstacleObject->GetRenderObject()->SetColour(colour);
	movingObstacleObject->SetPhysicsObject(new PhysicsObject(&movingObstacleObject->GetTransform(), movingObstacleObject->GetBoundingVolume()));

	movingObstacleObject->GetPhysicsObject()->SetInverseMass(inverseMass);
	movingObstacleObject->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(movingObstacleObject);
	movingObstacles.push_back(movingObstacleObject);

	return movingObstacleObject;
}

Goal* TutorialGame::AddGoalToWorld(const Vector3 & position, Vector3 dimensions, float inverseMass, string name, Vector4 colour)
{
	Goal* goalObject = new Goal();

	AABBVolume* volume = new AABBVolume(dimensions);

	goalObject->SetBoundingVolume((CollisionVolume*)volume);

	goalObject->GetTransform().SetWorldPosition(position);
	goalObject->GetTransform().SetWorldScale(dimensions);

	goalObject->SetRenderObject(new RenderObject(&goalObject->GetTransform(), cubeMesh, basicTex, basicShader));
	goalObject->GetRenderObject()->SetColour(colour);
	goalObject->SetPhysicsObject(new PhysicsObject(&goalObject->GetTransform(), goalObject->GetBoundingVolume()));

	goalObject->GetPhysicsObject()->SetInverseMass(inverseMass);
	goalObject->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(goalObject);

	return goalObject;
}

Player* TutorialGame::AddPlayerToWorld(const int id, const Vector3 & position, float radius, float inverseMass, string name, Vector4 colour, bool isHollow)
{
	Player* playerObject = new Player(id, name);

	Vector3 playerSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	playerObject->SetBoundingVolume((CollisionVolume*)volume);
	playerObject->GetTransform().SetWorldScale(playerSize);
	playerObject->GetTransform().SetWorldPosition(position);

	playerObject->SetRenderObject(new RenderObject(&playerObject->GetTransform(), sphereMesh, golfBallTex, basicShader));
	playerObject->GetRenderObject()->SetColour(colour);
	playerObject->SetPhysicsObject(new PhysicsObject(&playerObject->GetTransform(), playerObject->GetBoundingVolume()));

	playerObject->GetPhysicsObject()->SetInverseMass(inverseMass);

	if (isHollow)
		playerObject->GetPhysicsObject()->InitSphereInertia(true);
	else
		playerObject->GetPhysicsObject()->InitSphereInertia(false);

	playerObject->GetPhysicsObject()->SetElasticity(0.9f); // Higher elasticity => more bounciness

	world->AddGameObject(playerObject);

	return playerObject;
}

Robot* TutorialGame::AddRobotToWorld(const Vector3 & position, Vector3 dimensions, float inverseMass, string name, Vector4 colour)
{
	Robot* robotObject = new Robot();

	AABBVolume* volume = new AABBVolume(dimensions);

	robotObject->SetBoundingVolume((CollisionVolume*)volume);

	robotObject->GetTransform().SetWorldPosition(position);
	robotObject->GetTransform().SetWorldScale(dimensions);

	robotObject->SetRenderObject(new RenderObject(&robotObject->GetTransform(), cubeMesh, robotTex, basicShader));
	robotObject->GetRenderObject()->SetColour(colour);
	robotObject->SetPhysicsObject(new PhysicsObject(&robotObject->GetTransform(), robotObject->GetBoundingVolume()));

	robotObject->GetPhysicsObject()->SetInverseMass(inverseMass);
	robotObject->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(robotObject);

	return robotObject;
}

Sand* TutorialGame::AddSandToWorld(const Vector3 & position, Vector3 dimensions, float inverseMass, string name, Vector4 colour)
{
	Sand* sandObject = new Sand();

	AABBVolume* volume = new AABBVolume(dimensions);

	sandObject->SetBoundingVolume((CollisionVolume*)volume);

	sandObject->GetTransform().SetWorldPosition(position);
	sandObject->GetTransform().SetWorldScale(dimensions);

	sandObject->SetRenderObject(new RenderObject(&sandObject->GetTransform(), cubeMesh, sandTex, basicShader));
	sandObject->GetRenderObject()->SetColour(colour);
	sandObject->SetPhysicsObject(new PhysicsObject(&sandObject->GetTransform(), sandObject->GetBoundingVolume()));

	sandObject->GetPhysicsObject()->SetInverseMass(inverseMass);
	sandObject->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(sandObject);

	return sandObject;
}

void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius)
{
	for (int x = 0; x < numCols; ++x)
	{
		for (int z = 0; z < numRows; ++z)
		{
			Vector3 position = Vector3(x * colSpacing, radius, z * rowSpacing);
			AddSphereToWorld(position, radius, 200.0f, "Sphere");
		}
	}
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing)
{
	float sphereRadius = 10.0f;
	Vector3 cubeDims = Vector3(10, 10, 10);

	for (int x = 0; x < numCols; ++x)
	{
		for (int z = 0; z < numRows; ++z)
		{
			Vector3 position = Vector3(x * colSpacing, cubeDims.y, z * rowSpacing);

			if (rand() % 2)
			{
				AddCubeToWorld(position, cubeDims);
			}
			else
			{
				AddSphereToWorld(position, sphereRadius);
			}
		}
	}
}

void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims)
{
	for (int x = 0; x < numCols; ++x)
	{
		for (int z = 0; z < numRows; ++z)
		{
			Vector3 position = Vector3(x * colSpacing, cubeDims.y, z * rowSpacing);
			AddCubeToWorld(position, cubeDims, 1.0f);
		}
	}
}

void TutorialGame::InitSphereCollisionTorqueTest()
{
	AddSphereToWorld(Vector3(15, 0, 0), 10.0f);
	AddSphereToWorld(Vector3(-25, 0, 0), 10.0f);
	AddSphereToWorld(Vector3(-50, 0, 0), 10.0f);

	AddCubeToWorld(Vector3(-50, 0, -50), Vector3(60, 10, 10), 10.0f);
}


void TutorialGame::InitCubeCollisionTorqueTest()
{
	Vector3 cubeSize(10, 10, 10);
	AddCubeToWorld(Vector3(15, 0, 0), cubeSize, 10.0f);
	AddCubeToWorld(Vector3(-25, 0, 0), cubeSize, 10.0f);
	AddCubeToWorld(Vector3(-50, 0, 0), cubeSize, 10.0f);

	AddCubeToWorld(Vector3(-50, 0, -50), Vector3(60, 10, 10), 10.0f);
}

void TutorialGame::InitSphereAABBTest()
{
	Vector3 cubeSize(10, 10, 10);

	AddCubeToWorld(Vector3(0, 0, 0), cubeSize, 10.0f);
	AddSphereToWorld(Vector3(2, 0, 0), 5.0f, 10.0f);
}

void TutorialGame::InitGJKWorld()
{
	Vector3 dimensions(20, 2, 10);
	float inverseMass = 10.0f;

	for (int i = 0; i < 2; ++i)
	{
		GameObject* cube = new GameObject();

		OBBVolume* volume = new OBBVolume(dimensions);

		cube->SetBoundingVolume((CollisionVolume*)volume);

		cube->GetTransform().SetWorldPosition(Vector3(0, 0, 0));
		cube->GetTransform().SetWorldScale(dimensions);

		if (i == 1)
		{
			cube->GetTransform().SetLocalOrientation(Quaternion::AxisAngleToQuaternion(Vector3(1, 0, 0), 90.0f));
		}

		cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
		cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

		cube->GetPhysicsObject()->SetInverseMass(inverseMass);
		cube->GetPhysicsObject()->InitCubeInertia();

		world->AddGameObject(cube);
	}
}

void TutorialGame::BridgeConstraintTest()
{
	Vector3 cubeSize = Vector3(8, 8, 8);

	float invCubeMass = 5.0f; // How heavy the middle pieces are
	int numLinks = 10;
	float maxDistance = 30.0f; // Constraint distance
	float cubeDistance = 20.0f; // Distance between links

	Vector3 startPos = Vector3(500, 500, 500);

	GameObject* start = AddCubeToWorld(startPos + Vector3(0, 0, 0), cubeSize, 0);
	GameObject* end = AddCubeToWorld(startPos + Vector3((numLinks + 2) * cubeDistance, 0, 0), cubeSize, 0);
	GameObject* previous = start;

	for (int i = 0; i < numLinks; ++i)
	{
		GameObject* block = AddCubeToWorld(startPos + Vector3((i + 1) * cubeDistance, 0, 0), cubeSize, invCubeMass);
		PositionConstraint* constraint = new PositionConstraint(previous, block, maxDistance);
		world->AddConstraint(constraint);
		previous = block;
	}
	PositionConstraint* constraint = new PositionConstraint(previous, end, maxDistance);
	world->AddConstraint(constraint);
}

void TutorialGame::SimpleGJKTest()
{
	Vector3 dimensions = Vector3(5, 5, 5);
	Vector3 floorDimensions = Vector3(100, 2, 100);

	GameObject* fallingCube = AddCubeToWorld(Vector3(0, 20, 0), dimensions, 10.0f);
	GameObject* newFloor = AddCubeToWorld(Vector3(0, 0, 0), floorDimensions, 0.0f);

	delete fallingCube->GetBoundingVolume();
	delete newFloor->GetBoundingVolume();

	fallingCube->SetBoundingVolume((CollisionVolume*)new OBBVolume(dimensions));
	newFloor->SetBoundingVolume((CollisionVolume*)new OBBVolume(floorDimensions));
}

void TutorialGame::SimpleAABBTest()
{
	Vector3 dimensions = Vector3(5, 5, 5);
	Vector3 floorDimensions = Vector3(100, 2, 100);

	GameObject* newFloor = AddCubeToWorld(Vector3(0, 0, 0), floorDimensions, 0.0f);
	GameObject* fallingCube = AddCubeToWorld(Vector3(10, 20, 0), dimensions, 10.0f);
}

void TutorialGame::SimpleAABBTest2()
{
	Vector3 dimensions = Vector3(5, 5, 5);
	Vector3 floorDimensions = Vector3(8, 2, 8);

	GameObject* newFloor = AddCubeToWorld(Vector3(0, 0, 0), floorDimensions, 0.0f);
	GameObject* fallingCube = AddCubeToWorld(Vector3(8, 20, 0), dimensions, 10.0f);
}

void TutorialGame::UpdatePlayer()
{
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 50.0f;
	// Some boundary constraints
	if (forceMagnitude < 50.0f)
		forceMagnitude = 500.0f;
	if (forceMagnitude > 500.0f)
		forceMagnitude = 50.0f;

	vector<GameObject*>::const_iterator first;
	vector<GameObject*>::const_iterator last;
	world->GetObjectIterators(first, last);
	bool resetGame = false;

	for (auto it = first; it != last; ++it)
	{
		if ((*it)->GetName() == "Player")
		{
			/// TODO: Networking
			if (((Player*)(*it))->GetPlayerID() == currentPlayerID)
			{
				// Move the camera to the ball if the player decides so
				if (Window::GetKeyboard()->KeyPressed(KEYBOARD_B))
					world->GetMainCamera()->LookAt((*it)->GetTransform().GetWorldPosition());

				// If the ball is at "rest"-ish
				if ((*it)->GetPhysicsObject()->GetLinearVelocity() < Vector3(10.0f, 10.0f, 10.0f))
				{
					// Strike it!
					if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::MOUSE_LEFT))
					{
						Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

						RayCollision closestCollision;
						if (world->Raycast(ray, closestCollision, true))
						{
							if (!(*it)->GetPhysicsObject())
								return;

							if (closestCollision.node == (*it))
							{
								(*it)->GetPhysicsObject()->AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt);
								((Player*)(*it))->SetHitCounter(((Player*)(*it))->GetHitCounter() + 1);
							}
						}
					}
				}
				// Draw stuff related to the Player on screen
				renderer->DrawString("Shooting Force: " + std::to_string((int)trunc(forceMagnitude)), Vector2(10, 10));
				int hitCounter = (int)trunc(((Player*)(*it))->GetHitCounter());
				DrawHitCounter(hitCounter);

				// If the player goes out of bounds, they die
				if ((*it)->GetTransform().GetWorldPosition().y < -600.0f)
					(*it)->Kill();
				// If a player is dead, reset the game level
				if (!(*it)->IsAlive())
					resetGame = true;
				// If the player finished a level, go to the next one (if there is one)
				if (((Player*)(*it))->IsLevelCleared())
				{
					switch (gameState->GetLevel())
					{
					case LEVEL_1:
						gameState->SetLevel(LEVEL_2);
						break;
					case LEVEL_2:
						gameState->SetLevel(END_SCENE);
						break;
					}
					resetGame = true;
				}
			}
		}
	}
	if (resetGame)
		ResetGame();
}

void TutorialGame::UpdateRobot()
{
	UpdatePathfinding();
}

void TutorialGame::UpdatePathfinding()
{
	if (gameState->GetLevel() == MAIN_MENU || gameState->GetLevel() == END_SCENE)
		return;

	vector<GameObject*>::const_iterator first;
	vector<GameObject*>::const_iterator last;
	world->GetObjectIterators(first, last);

	NavigationPath outPath;
	vector<Vector3> pathNodes;
	Vector3 startPos;
	Vector3 endPos;

	for (auto it = first; it != last; ++it)
	{
		if ((*it)->GetName() == "Robot")
		{
			startPos = (*it)->GetTransform().GetWorldPosition();

			((Robot*)(*it))->SetPlayerInRange(isPlayerInRange);
			((Robot*)(*it))->GetStateMachine()->Update();
		}

		if ((*it)->GetName() == "Player")
			endPos = (*it)->GetTransform().GetWorldPosition();
	}

	CompareRobotAndPlayer(startPos, endPos);

	NavigationGrid* currentLevel = levelGrids.at(gameState->GetLevel() - 1);
	bool pathFound = currentLevel->FindPath(startPos, endPos, outPath);

	Vector3 pos;
	while (outPath.PopWaypoint(pos))
		pathNodes.push_back(pos);

	MoveRobot(pathFound, pathNodes, first, last);

	DisplayPathfinding(pathNodes);
}

void TutorialGame::MoveRobot(bool pathFound, vector<Vector3>& pathNodes, vector<GameObject*>::const_iterator& first, vector<GameObject*>::const_iterator& last)
{
	if (pathFound) // If a path to the ball is found
	{
		for (auto it = first; it != last; ++it) // Find the Robot again
		{
			if ((*it)->GetName() == "Robot")
			{
				// And if its state is right (i.e. The ball is inside its detection range)
				if (((Robot*)(*it))->GetStateMachine()->GetActiveState()->GetName() == "Chase") 
				{
					if (!pathNodes.empty() && (*it)->GetTransform().GetWorldPosition().x != 0.0f)
					{
						Vector3 currentPos = (*it)->GetTransform().GetWorldPosition();
						Vector3 nextPos;

						if (pathNodes.size() >= 2) // Baka -_-
							nextPos = pathNodes.at(1);
						else
							nextPos = Vector3(0, 0, 0);
						Vector3 directionVector = Vector3(nextPos.x - currentPos.x, 0.0f, nextPos.z - currentPos.z);
						directionVector.Normalise();

						(*it)->GetPhysicsObject()->ApplyLinearImpulse(directionVector * robotSpeed);
					}
					break;
				}
			}
		}
	}
}

void TutorialGame::CompareRobotAndPlayer(Vector3& robotPos, Vector3& playerPos)
{
	Vector3 temp = robotPos - playerPos;
	float distance = abs(temp.Length());
	if (distance < robotDetectionRange)
		isPlayerInRange = true;
	else
		isPlayerInRange = false;
}

void TutorialGame::DisplayPathfinding(vector<Vector3> & pathNodes)
{
	for (int i = 1; i < (int)pathNodes.size(); ++i)
	{
		Vector3 a = pathNodes[i - 1];
		Vector3 b = pathNodes[i];

		if (drawPathfindingLines)
			renderer->DrawLine(a, b, Vector4(0, 1, 0.5f, 1));
	}
}

void TutorialGame::ResetScenes()
{
	renderEndScene = false;
	renderMenu = false;
}
