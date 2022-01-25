#pragma once
#include "Audio/Audio.h"
#include "PhysicsSystem.h"

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

	Audio* audio;
	PhysicsSystem* physics;

private:
};