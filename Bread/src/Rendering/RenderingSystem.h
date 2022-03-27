#pragma once

#include <vector>

#include "Shader.h"
#include "Model.h"
#include "DebugOverlay.h"
#include "../Transform.h"
#include "../Navigation/NavMesh.h"

struct Orthogonal
{
	float left;
	float right;
	float bottom;
	float top;

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

	void updateOrtho(glm::mat4 lightView);
	void updateRoughOrtho();
	glm::mat4 calculateOrthoProjection();

	Orthogonal ort;
	glm::vec3 lightPos;
	glm::vec3 lightDir;
	float shadowHiRes;
	float shadowLoRes;

	int shadowDebugMode;

	unsigned int depthMapTex;

	float maxBias;
	float minBias;
	float maxRoughBias;
	float minRoughBias;

	glm::mat4 projMatrix;
	glm::mat4 viewMatrix;

private:

	//DebugOverlay& debugOverlay;

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
	Shader skyboxShader;

	std::vector<Model> models;

	Mesh navMesh;

	unsigned int FBO;
	unsigned int textureColorBuffer;
	unsigned int RBO;
	unsigned int quadVAO;
	unsigned int quadVBO;

	unsigned int roughDepthMapTex;
	unsigned int roughDepthMapFBO;
	unsigned int roughDepthMapVAO;
	unsigned int depthMapFBO;
	unsigned int dpethMapVAO;

	unsigned int cubeMap;
	unsigned int skyboxVAO;
	unsigned int skyboxVBO;

	void drawSkybox();
};