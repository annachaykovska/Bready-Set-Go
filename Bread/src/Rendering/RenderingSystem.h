#pragma once

#include <vector>

#include "Shader.h"
#include "Model.h"
#include "../Transform.h"

struct Orthogonal
{
	float x;
	float y;
	float z;
	float w;

	float nearPlane;
	float farPlane;
};

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
	void renderScene();
	void renderShadowMap();
	void renderTexturedQuad();
	void renderDebugShadowMap();

	Orthogonal ort;
	glm::vec3 lightPos;
	glm::vec3 lightDir;
	float shadowWidth;
	float shadowHeight;

	unsigned int depthMapTex;


private:

	unsigned int modelLoc;
	unsigned int texLoc;
	unsigned int viewLoc;
	unsigned int projLoc;
	
	Shader shader;
	Shader lightShader;
	Shader borderShader;
	Shader simpleShader;
	Shader depthShader;
	Shader debugShader;

	std::vector<Model> models;

	unsigned int FBO;
	unsigned int textureColorBuffer;
	unsigned int RBO;
	unsigned int quadVAO;
	unsigned int quadVBO;

	unsigned int depthMapFBO;
	unsigned int dpethMapVAO;

	int screenWidth;
	int screenHeight;
};