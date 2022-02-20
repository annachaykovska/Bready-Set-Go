#pragma once

#include <vector>

#include "Shader.h"
#include "Model.h"
#include "DebugOverlay.h"
#include "../Transform.h"

class RenderingSystem
{
public:

	RenderingSystem(DebugOverlay& debug);
	~RenderingSystem();
	unsigned int getShaderId();
	Shader& getShader();
	void loadModels();
	void setupCameras(Transform* playerTransform);
	void update();

private:

	DebugOverlay& debugOverlay;

	unsigned int modelLoc;
	unsigned int texLoc;
	unsigned int viewLoc;
	unsigned int projLoc;
	Shader shader;
	std::vector<Model> models;
	
};