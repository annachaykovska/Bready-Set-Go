#pragma once
#include "Navigation/IngredientTracker.h"
//#include "Gameplay/GameLoopManager.h"
//#include "Audio/AudioSystem.h"
//#include "Physics/PhysicsSystem.h"
//#include "Rendering/RenderingSystem.h"

class AudioSystem;
class PhysicsSystem;
class RenderingSystem;
class UISystem;
class GameLoopManager;
class Profiler;

class SystemManager
{
public:

	SystemManager()
	{
		audio = nullptr;
		physics = nullptr;
		render = nullptr;
		ui = nullptr;
		renderDebug = false;
		tracker = nullptr;
		loop = nullptr;
		profiler = nullptr;

		width = 0;
		height = 0;
	}

	SystemManager(SystemManager& rhs) = delete; // No copy constructor
	void operator=(const SystemManager&) = delete; // No assignment operator

	AudioSystem* audio;
	PhysicsSystem* physics;
	RenderingSystem* render;
	UISystem* ui;
	IngredientTracker* tracker;
	GameLoopManager* loop;
	Profiler* profiler;

	int height;
	int width;
	bool renderDebug;
};