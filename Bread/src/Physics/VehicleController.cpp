/*
* Keyboard and Controller callback handler
*/
#include <vector>
#include <memory>

#include "../Window.h"
#include "../Scene/Scene.h"
#include "PhysicsSystem.h"

using namespace std;

extern Scene g_scene;


class MovementCallbacks : public CallbackInterface {

public:
	// Currently just taking a pointer to the main player transform????
	MovementCallbacks(PhysicsSystem* physics) : physics(physics) {
		state = PhysicsSystem::ButtonState();

	}

	virtual void keyCallback(int key, int scancode, int action, int mods) {
		// Pressing settings
		if (state.forwardsHeld || (key == GLFW_KEY_W && action == GLFW_PRESS)) {
			state.forwardsHeld = true;
		}
		if (state.leftHeld || (key == GLFW_KEY_A && action == GLFW_PRESS)) {
			state.leftHeld = true;
		}
		if (state.backwardsHeld || (key == GLFW_KEY_S && action == GLFW_PRESS)) {
			state.backwardsHeld = true;
		}
		if (state.rightHeld || (key == GLFW_KEY_D && action == GLFW_PRESS)) {
			state.rightHeld = true;
		}

		Camera* camera = &(g_scene.camera);

		// Debug camera controls
		if (state.cameraForward || key == GLFW_KEY_I && action == GLFW_PRESS) {
			state.cameraForward = true;
			camera->position.x += 0.5;
		}
		if (state.cameraBackward || key == GLFW_KEY_K && action == GLFW_PRESS) {
			state.cameraBackward = true;
			camera->position.x -= 0.5;
		}
		if (state.cameraRight || key == GLFW_KEY_L && action == GLFW_PRESS) {
			state.cameraRight = true;
			camera->position.z += 0.5;
		}
		if (state.cameraLeft || key == GLFW_KEY_J && action == GLFW_PRESS) {
			state.cameraLeft = true;
			camera->position.z -= 0.5;
		}
		if (state.cameraUp || key == GLFW_KEY_O && action == GLFW_PRESS) {
			state.cameraUp = true;
			camera->position.y += 0.5;
		}
		if (state.cameraDown || key == GLFW_KEY_U && action == GLFW_PRESS) {
			state.cameraDown = true;
			camera->position.y -= 0.5;
		}

		// Release settings
		if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
			state.forwardsHeld = false;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
			state.leftHeld = false;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
			state.backwardsHeld = false;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
			state.rightHeld = false;
		}
		if (key == GLFW_KEY_I && action == GLFW_RELEASE) {
			state.cameraForward = false;
		}
		if (key == GLFW_KEY_K && action == GLFW_RELEASE) {
			state.cameraBackward = false;
		}
		if (key == GLFW_KEY_L && action == GLFW_RELEASE) {
			state.cameraRight = false;
		}
		if (key == GLFW_KEY_J && action == GLFW_RELEASE) {
			state.cameraLeft = false;
		}
		if (key == GLFW_KEY_O && action == GLFW_RELEASE) {
			state.cameraUp = false;
		}
		if (key == GLFW_KEY_U && action == GLFW_RELEASE) {
			state.cameraDown = false;
		}


		// Set the physics key callback
		if (action == GLFW_PRESS)
		{
			physics->keyPress(key);
		}

		if (action == GLFW_RELEASE)
		{
			physics->keyRelease(key);
		}

		// Update keypress
		if (action == GLFW_PRESS)
		{
			physics->keyPress(key);
		}
		
		if (action == GLFW_RELEASE)
		{
			physics->keyRelease(key);
		}
	}

private:
	PhysicsSystem::ButtonState state;
	PhysicsSystem* physics;
};