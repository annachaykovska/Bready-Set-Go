#include <vector>
#include <memory>
#include <Windows.h>
#include <xinput.h>

#include "../Window.h"
#include "../Scene/Scene.h"
#include "PhysicsSystem.h"

#pragma comment(lib, "Xinput.lib") 
#pragma comment(lib, "Xinput9_1_0.lib")

extern Scene g_scene;


class MovementCallbacks : public CallbackInterface {

public:
	// Currently just taking a pointer to the main player transform????
	MovementCallbacks(PhysicsSystem* physics) : physics(physics) {
	}

	virtual void keyCallback(int key, int scancode, int action, int mods) {
		// Pressing settings
		if (physics->buttonState.forwardsHeld || (key == GLFW_KEY_W && action == GLFW_PRESS)) {
			physics->buttonState.forwardsHeld = true;
		}
		if (physics->buttonState.leftHeld || (key == GLFW_KEY_A && action == GLFW_PRESS)) {
			physics->buttonState.leftHeld = true;
		}
		if (physics->buttonState.backwardsHeld || (key == GLFW_KEY_S && action == GLFW_PRESS)) {
			physics->buttonState.backwardsHeld = true;
		}
		if (physics->buttonState.rightHeld || (key == GLFW_KEY_D && action == GLFW_PRESS)) {
			physics->buttonState.rightHeld = true;
		}
		if (physics->buttonState.brakeHeld || (key == GLFW_KEY_SPACE && action == GLFW_PRESS)) {
			physics->buttonState.brakeHeld = true;
		}

		Camera* camera = &(g_scene.camera);

		// Debug camera controls
		if (physics->buttonState.cameraForward || key == GLFW_KEY_I && action == GLFW_PRESS) {
			physics->buttonState.cameraForward = true;
			camera->position.x += 0.5;
		}
		if (physics->buttonState.cameraBackward || key == GLFW_KEY_K && action == GLFW_PRESS) {
			physics->buttonState.cameraBackward = true;
			camera->position.x -= 0.5;
		}
		if (physics->buttonState.cameraRight || key == GLFW_KEY_L && action == GLFW_PRESS) {
			physics->buttonState.cameraRight = true;
			camera->position.z += 0.5;
		}
		if (physics->buttonState.cameraLeft || key == GLFW_KEY_J && action == GLFW_PRESS) {
			physics->buttonState.cameraLeft = true;
			camera->position.z -= 0.5;
		}
		if (physics->buttonState.cameraUp || key == GLFW_KEY_O && action == GLFW_PRESS) {
			physics->buttonState.cameraUp = true;
			camera->position.y += 0.5;
		}
		if (physics->buttonState.cameraDown || key == GLFW_KEY_U && action == GLFW_PRESS) {
			physics->buttonState.cameraDown = true;
			camera->position.y -= 0.5;
		}

		// Release settings
		if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
			physics->buttonState.forwardsHeld = false;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
			physics->buttonState.leftHeld = false;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
			physics->buttonState.backwardsHeld = false;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
			physics->buttonState.rightHeld = false;
		}
		if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
			physics->buttonState.brakeHeld = false;
		}
		if (key == GLFW_KEY_I && action == GLFW_RELEASE) {
			physics->buttonState.cameraForward = false;
		}
		if (key == GLFW_KEY_K && action == GLFW_RELEASE) {
			physics->buttonState.cameraBackward = false;
		}
		if (key == GLFW_KEY_L && action == GLFW_RELEASE) {
			physics->buttonState.cameraRight = false;
		}
		if (key == GLFW_KEY_J && action == GLFW_RELEASE) {
			physics->buttonState.cameraLeft = false;
		}
		if (key == GLFW_KEY_O && action == GLFW_RELEASE) {
			physics->buttonState.cameraUp = false;
		}
		if (key == GLFW_KEY_U && action == GLFW_RELEASE) {
			physics->buttonState.cameraDown = false;
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
	PhysicsSystem* physics;
};


class XboxController {
public:
	XboxController(PhysicsSystem* physicsSystem);
	void setControllers();
	void setButtonStateFromController(int controllerId);
	_XINPUT_STATE getControllerState(int controllerId);
	void getDeadZone(float x, float y, float deadzone);

private:
	PhysicsSystem* physics;
};