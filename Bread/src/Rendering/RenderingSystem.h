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
	Shader simpleShader;
	std::vector<Model> models;
	unsigned int FBO;
	unsigned int textureColorBuffer;
	unsigned int RBO;
	unsigned int quadVAO;
	unsigned int quadVBO;

	unsigned int depthMapFBO;
	unsigned int depthMap;

	int screenWidth;
	int screenHeight;
};