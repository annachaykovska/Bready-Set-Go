#pragma once
#include "Audio/AudioSystem.h"
#include "PhysicsSystem.h"
#include "Rendering/RenderingSystem.h"

class AudioSystem;
class PhysicsSystem;
class RenderingSystem;

class SystemManager
{
public:

	SystemManager()
	{
		audio = nullptr;
		physics = nullptr;
		render = nullptr;
	}

	SystemManager(SystemManager& rhs) = delete; // No copy constructor
	void operator=(const SystemManager&) = delete; // No assignment operator

	AudioSystem* audio;
	PhysicsSystem* physics;
	RenderingSystem* render;

private:
};