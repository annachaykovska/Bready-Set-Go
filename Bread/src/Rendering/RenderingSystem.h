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

	void update();

	unsigned int getShaderId();
	Shader& getShader();

	void setupCameras(Transform* playerTransform);

	Model* getKitchenModel();

	Orthogonal roughOrt;
	glm::mat4 lightViewMatrix;
	glm::mat4 loResLightSpaceMatrix;
	glm::mat4 p1LightSpaceMatrix;
	glm::mat4 p2LightSpaceMatrix;
	glm::mat4 p3LightSpaceMatrix;
	glm::mat4 p4LightSpaceMatrix;

	glm::vec3 lightPos;
	glm::vec3 lightDir;
	int shadowHiRes;
	int shadowLoRes;

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

	// List of all the models to be rendered
	std::vector<Model> models;

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

	unsigned int p1ShadowsFBO;
	unsigned int p1ShadowsTex;
	unsigned int p2ShadowsFBO;
	unsigned int p2ShadowsTex;
	unsigned int p3ShadowsFBO;
	unsigned int p3ShadowsTex;
	unsigned int p4ShadowsFBO;
	unsigned int p4ShadowsTex;

	unsigned int fourPlayerVAO;
	unsigned int fourPlayerVBO;
	unsigned int fourPlayerFBO;
	unsigned int fourPlayerTex;
	unsigned int fourPlayerRBO;

	void initShadows();
	void initSkybox();
	void loadModels();
	void initDebugQuad();
	void init4PlayerQuad();

	void createLoResShadowMap();
	void createHiResShadowMap(const std::string name);
	void renderShadowMap();

	void renderScene(const std::string name);

	void drawSkybox();

	void renderTexturedQuad();
	void renderOnePlayerQuad();
	void renderFourPlayerQuad();
	void renderDebugShadowMap();
};