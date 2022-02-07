#pragma once

#include <vector>

#include "Shader.h"
#include "Model.h"
#include "../Transform.h"

class RenderingSystem
{
public:

	unsigned int modelLoc;
	unsigned int texLoc;

	unsigned int viewLoc;
	unsigned int projLoc;

	RenderingSystem();
	unsigned int getShaderId();
	Shader& getShader();
	void loadModels();
	void setupCameras(Transform* playerTransform);
	void update();

	Model player1;
	Model player2;
	Model countertop;

private:

	Shader shader;
	std::vector<Model> models;
	
};