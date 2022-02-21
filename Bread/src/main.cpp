#include <iostream>
#include <vector>

#include "Window.h"
#include "Profiler.h"
#include "SystemManager.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Transform.h"
#include "Rendering/DebugOverlay.h"
#include "Rendering/RenderingSystem.h"
#include "Audio/AudioSystem.h"
#include "Navigation/NavMesh.h"
#include "Navigation/NavigationSystem.h"
#include "Physics/VehicleController.cpp"
#include "Inventory.h"
#include "DebugSettings.h"

// Global Scene holds all the Entities for easy reference
Scene g_scene;

// Global SystemManager holds all the Systems for easy reference
SystemManager g_systems;

int main()
{
	// Initialize GLFW library
	glfwInit();

	// Create viewport window
	Window window(800, 600, "Bready Set Go!!!");
	window.setBackgroundColor(0.6784f, 0.8471f, 0.902f);

	// ImGui profiler for debugging
	Profiler profiler(window);

	//-----------------------------------------------------------------------------------
	// ENTITY-COMPONENT STUFF 
	//-----------------------------------------------------------------------------------
	// Populate the scene with Entities (will happen on game start)
	// This happens all at once to avoid dangling pointers in the future
	g_scene.createEntities();

	// Get references to Entities for easy use and set vehicle physx objects
	Entity* player1 = g_scene.getEntity("player1");
	Entity* player2 = g_scene.getEntity("player2");
	Entity* player3 = g_scene.getEntity("player3");
	Entity* player4 = g_scene.getEntity("player4");
	Entity* cheese = g_scene.getEntity("cheese");
	Entity* sausage = g_scene.getEntity("sausage");
	Entity* tomato = g_scene.getEntity("tomato");
	Entity* dough = g_scene.getEntity("dough");
	Entity* countertop = g_scene.getEntity("countertop");
	Entity* demoObstacle = g_scene.getEntity("demoObstacle");

	//-----------------------------------------------------------------------------------
	// INITIALIZE SYSTEMS
	//-----------------------------------------------------------------------------------
	PhysicsSystem physics;
	g_systems.physics = &physics;
	player1->vehicle = physics.mVehiclePlayer1; // Don't think this is needed?

	DebugOverlay debugOverlay;
	RenderingSystem renderer(debugOverlay);
	g_systems.render = &renderer;

	AudioSystem audio;
	g_systems.audio = &audio;

	//-----------------------------------------------------------------------------------
	// INITIALIZE TRANSFORMS - will be handled by PhysicsSystem eventually
	//-----------------------------------------------------------------------------------
	// Create a container for Transform Components (will be handled by a system in the future)
	// and add some some new Transforms to it for the Entities
	std::vector<Transform> transforms;
	for (int i = 0; i < 10; i++)
		transforms.emplace_back(Transform());

	// Attach one of the Transform Components to each Entity in the Scene
	player1->attachComponent(&transforms[0], "transform");
	player2->attachComponent(&transforms[1], "transform");
	player3->attachComponent(&transforms[2], "transform");
	player4->attachComponent(&transforms[3], "transform");
	cheese->attachComponent(&transforms[4], "transform");
	sausage->attachComponent(&transforms[5], "transform");
	tomato->attachComponent(&transforms[6], "transform");
	dough->attachComponent(&transforms[7], "transform");
	countertop->attachComponent(&transforms[8], "transform");
	demoObstacle->attachComponent(&transforms[9], "transform");

	// Initialize transform components
	Transform* player1Transform = player1->getTransform();
	player1Transform->position = glm::vec3(2.f, 2.3f, -3.f);
	player1Transform->rotation = glm::vec3(0, 0, 0);
	player1Transform->scale = glm::vec3(2, 2, 2);

	Transform* player2Transform = player2->getTransform();
	player2Transform->position = glm::vec3(100, 2.3f, 0);
	player2Transform->rotation = glm::vec3(0, 0, 0);
	player2Transform->scale = glm::vec3(1, 1, 1);

	Transform* player3Transform = player3->getTransform();
	player3Transform->position = glm::vec3(-100, 2.3f, 0);
	player3Transform->rotation = glm::vec3(0, 0, 0);
	player3Transform->scale = glm::vec3(1, 1, 1);

	Transform* player4Transform = player4->getTransform();
	player4Transform->position = glm::vec3(0, 2.3f, -100);
	player4Transform->rotation = glm::vec3(0, 0, 0);
	player4Transform->scale = glm::vec3(1, 1, 1);

	Transform* cheeseTransform = cheese->getTransform();
	cheeseTransform->position = glm::vec3(20, 5, 20);
	cheeseTransform->rotation = glm::vec3(0, 0, 0);
	cheeseTransform->scale = glm::vec3(30, 30, 30);

	Transform* sausageTransform = sausage->getTransform();
	sausageTransform->position = glm::vec3(-20, 5, 20);
	sausageTransform->rotation = glm::vec3(0, 0, 0);
	sausageTransform->scale = glm::vec3(30, 30, 30);

	Transform* tomatoTransform = tomato->getTransform();
	tomatoTransform->position = glm::vec3(20, 5, -20);
	tomatoTransform->rotation = glm::vec3(0, 0, 0);
	tomatoTransform->scale = glm::vec3(30, 30, 30);

	Transform* doughTransform = dough->getTransform();
	doughTransform->position = glm::vec3(-20, 5, -20);
	doughTransform->rotation = glm::vec3(0, 0, 0);
	doughTransform->scale = glm::vec3(30, 30, 30);

	Transform* counterTransform = countertop->getTransform();
	counterTransform->position = glm::vec3(0, 0, 0);
	counterTransform->rotation = glm::vec3(0, 0, 0);
	counterTransform->scale = glm::vec3(50, 50, 50);

	Transform* demoObstacleTransform = demoObstacle->getTransform();
	demoObstacleTransform->position = glm::vec3(0, 0, 0);
	demoObstacleTransform->rotation = glm::vec3(0, 0, 0);
	demoObstacleTransform->scale = glm::vec3(10, 10, 10);
	//-----------------------------------------------------------------------------------

	// Get a reference to the countertop's AudioSource to play background music
	// Leaving this here for now so people can turn the music on/off easily
	AudioSource* countertopAudioSource = countertop->getAudioSource();
	countertopAudioSource->gain = 0.01f; // Volume control
	countertopAudioSource->play("bg.wav"); // Comment this out to turn off the music on load

	// Track time
	float oldTime = glfwGetTime(), newTime = 0, deltaTime = 0;

	// Set movement control callbacks
	// TODO: generalize this
	auto movementCallbacks = std::make_shared<MovementCallbacks>(&physics);
	window.setCallbacks(movementCallbacks);

	//-----------------------------------------------------------------------------------
	// GameLogic stuff - will go in GameLogic eventually
	//-----------------------------------------------------------------------------------
	Inventory p1Inv;
	NavMesh navMesh;
	NavigationSystem p1NavSystem(*player1, physics, navMesh);

	debugOverlay.addDebugMesh(navMesh.getWireframe(), DEBUG_NAV_MESH);

	player1->attachComponent(&p1Inv, "inventory");
	player1->attachComponent(&p1NavSystem, "navigation");

	// TODO: Dynamically pick the target and plan path
	p1NavSystem.planPath(glm::vec3(95.f, 0.f, -95.f));

	Shader shader("resources/shaders/vertex.txt", "resources/shaders/fragment.txt");
	unsigned int modelLoc = glGetUniformLocation(shader.getId(), "model");
	unsigned int texLoc = glGetUniformLocation(shader.getId(), "textured");

	// GAME LOOP
	while (!window.shouldClose())
	{
		// INPUT
		glfwPollEvents();

		// SIMULATE
		newTime = glfwGetTime();
		deltaTime = newTime - oldTime;
		g_systems.physics->update(deltaTime);
		oldTime = newTime;

		// RENDER
		window.clear();
		renderer.update();
		profiler.newFrame();

		// Update the ImGUI profiler
		profiler.update();

		// Swap the frame buffers
		window.swapBuffer();

		// AI
		p1NavSystem.update();

		// AUDIO
		// update AudioSource

	}

	// Collect garbage
	physics.cleanupPhysics();
	profiler.cleanup();
	renderer.getShader().cleanup();
	glfwTerminate();

	return 0;
}