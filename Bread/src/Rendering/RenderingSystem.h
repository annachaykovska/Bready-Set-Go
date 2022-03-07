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
	Model* getKitchenModel();

private:

	unsigned int modelLoc;
	unsigned int texLoc;
	unsigned int viewLoc;
	unsigned int projLoc;
	Shader shader;
	Shader lightShader;
	Shader borderShader;
	std::vector<Model> models;
	
};