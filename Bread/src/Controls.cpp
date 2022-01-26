/*
* Keyboard and Controller callback handler
*/
#include <vector>
#include "Component.h"

using namespace std;
/*
* List of TODOs:
*		- Start with one player input
*		- Track which players input is using the callback 
*/

class MovementCallbacks : public CallbackInterface {

public:
	// Currently just taking a pointer to the main player transform????
	MovementCallbacks(shared_ptr<Transform> transform) : transform(transform) {}

	virtual void keyCallback(int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_W && action == GLFW_PRESS) {
			printf("W");
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS) {
			printf("A");
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS) {
			printf("S");
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS) {
			printf("D");
		}
	}

	

private:
	shared_ptr<Transform> transform;
};