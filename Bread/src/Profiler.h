#pragma once

class Window;

class Profiler {
public:

	Profiler(Window& window);
	void newFrame();
	void cleanup();
	void begin();
	void end();
	void fps();
	void player1Inventory();
	void player1Transform();
	void player2Transform();
	void player3Transform();
	void player4Transform();
	void cameraTransform();
	void testTransform();
	void update();

private:

	unsigned int frameCounter;
	double t0;
	double msPerFrame;
	double framesPerSecond;
};