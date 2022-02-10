#pragma once

#include <vector>

#include "Shader.h"
#include "Model.h"
#include "../Transform.h"

class RenderingSystem
{
public:

	RenderingSystem();
	~RenderingSystem();
	unsigned int getShaderId();
	Shader& getShader();
	void loadModels();
	void setupCameras(Transform* playerTransform);
	void update();

	Model player1;
	Model player2;
	Model player3;
	Model player4;
	Model cheese;
	Model tomato;
	Model sausage;
	Model dough;
	Model countertop;

private:

	unsigned int modelLoc;
	unsigned int texLoc;
	unsigned int viewLoc;
	unsigned int projLoc;
	Shader shader;
	std::vector<Model> models;
	
};