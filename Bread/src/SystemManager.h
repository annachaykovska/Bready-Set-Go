#pragma once
#include "Audio/AudioSystem.h"
#include "Physics/PhysicsSystem.h"

class SystemManager
{
public:

	SystemManager()
	{
		audio = nullptr;
		physics = nullptr;
	}

	SystemManager(SystemManager& rhs) = delete; // No copy constructor
	void operator=(const SystemManager&) = delete; // No assignment operator

	AudioSystem* audio;
	PhysicsSystem* physics;

private:
};