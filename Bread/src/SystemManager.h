#pragma once
//#include "Audio/AudioSystem.h"
//#include "Physics/PhysicsSystem.h"
//#include "Rendering/RenderingSystem.h"

class AudioSystem;
class PhysicsSystem;
class RenderingSystem;
class UISystem;

class SystemManager
{
public:

	SystemManager()
	{
		audio = nullptr;
		physics = nullptr;
		render = nullptr;
		ui = nullptr;
	}

	SystemManager(SystemManager& rhs) = delete; // No copy constructor
	void operator=(const SystemManager&) = delete; // No assignment operator

	AudioSystem* audio;
	PhysicsSystem* physics;
	RenderingSystem* render;
	UISystem* ui;

private:
};