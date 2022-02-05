/*
* Keyboard and Controller callback handler
*/
#include <vector>
#include <memory>

#include <glfw/glfw3.h>

#include "../Window.h"
#include "PhysicsSystem.h"

using namespace std;

struct ButtonState {
	bool forwardsHeld = false;
	bool leftHeld = false;
	bool backwardsHeld = false;
	bool rightHeld = false;
};
	

class MovementCallbacks : public CallbackInterface {

public:
	// Currently just taking a pointer to the main player transform????
	MovementCallbacks(PhysicsSystem* physics) : physics(physics) {
		state = ButtonState();

	}

	virtual void keyCallback(int key, int scancode, int action, int mods) {
		// Pressing settings
		//if (state.forwardsHeld || (key == GLFW_KEY_W && action == GLFW_PRESS)) {
		//	transform->position.x += 0.5;
		//	state.forwardsHeld = true;
		//	printf("W");
		//}
		//if (state.leftHeld || (key == GLFW_KEY_A && action == GLFW_PRESS)) {
		//	transform->position.z -= 0.5;
		//	state.leftHeld = true;
		//	printf("A");
		//}
		//if (state.backwardsHeld || (key == GLFW_KEY_S && action == GLFW_PRESS)) {
		//	transform->position.x -= 0.5;
		//	state.backwardsHeld = true;
		//	printf("S");
		//}
		//if (state.rightHeld || (key == GLFW_KEY_D && action == GLFW_PRESS)) {
		//	transform->position.z += 0.5;
		//	state.rightHeld = true;
		//	printf("D");
		//}

		//// Release settings
		//if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
		//	state.forwardsHeld = false;
		//}
		//if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
		//	state.leftHeld = false;
		//}
		//if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
		//	state.backwardsHeld = false;
		//}
		//if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
		//	state.rightHeld = false;
		//}

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
	ButtonState state;
	PhysicsSystem* physics;
};