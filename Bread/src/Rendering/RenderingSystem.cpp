#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

#include "../SystemManager.h"
#include "RenderingSystem.h"
#include "../Scene/Scene.h"
#include "../Scene/Entity.h"

extern Scene g_scene;
extern SystemManager g_systems;

RenderingSystem::RenderingSystem() : shader("resources/shaders/vertex.txt", "resources/shaders/fragment.txt"),
									 lightShader("resources/shaders/lightSourceVertex.txt", "resources/shaders/lightSourceFragment.txt"),
									 borderShader("resources/shaders/lightSourceVertex.txt", "resources/shaders/borderFragment.txt"),
								     simpleShader("resources/shaders/simpleVertex.txt", "resources/shaders/simpleFragment.txt"),
									 depthShader("resources/shaders/depthVertex.txt", "resources/shaders/depthFragment.txt"),
									 debugShader("resources/shaders/debugVertex.txt", "resources/shaders/debugFragment.txt")
{
	// Orthographic projection for shadow map settings
	this->ort.left = 281.0f;
	this->ort.right = -279.0f;
	this->ort.bottom = -205.0f;
	this->ort.top = 197.0f;
	this->ort.nearPlane = 0.1f;
	this->ort.farPlane = 600.0f;

	// Directional light position
	this->lightPos = glm::vec3(50.0f, 200.0f, 200.0f);
	//this->lightDir = glm::vec3(1.0f, -1.0f, 1.0f);

	// Shadow map viewport size
	this->shadowWidth = 4096;
	this->shadowHeight = 4096;

	this->maxBias = 0.001f;
	this->minBias = 0.00005f;
	this->maxRoughBias = 0.005f;
	this->minRoughBias = 0.0005f;

	this->models.reserve(g_scene.entityCount() * 2); // Create space for models
	loadModels(); // Load model files into the models vector

	glEnable(GL_DEPTH_TEST); // Turn on depth testing
	glDepthFunc(GL_LESS); // Should be default but make it explicit

	glEnable(GL_CULL_FACE); // Enable face culling
	glCullFace(GL_BACK); // Turn on back-face culling initially

	// Rendering uniforms
	shader.use();
	this->modelLoc = glGetUniformLocation(getShaderId(), "model");
	this->texLoc = glGetUniformLocation(getShaderId(), "textured");
	this->viewLoc = glGetUniformLocation(getShaderId(), "view");
	this->projLoc = glGetUniformLocation(getShaderId(), "proj");

	//glEnable(GL_STENCIL_TEST); // Turn on stencil testing
	//glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// Camera
	Transform transform = Transform();
	transform.position = glm::vec3(1.0f);
	setupCameras(&transform); // Setup the camera
	
	// --------------------------------------------------------------------------------------------
	// SHADOWS
	// --------------------------------------------------------------------------------------------
	// Rough shadows ------------------------------------------------------------------------------
	// Configure depth map FBO
	glGenFramebuffers(1, &this->roughDepthMapFBO);

	// Create depth texture
	glGenTextures(1, &this->roughDepthMapTex);
	glActiveTexture(GL_TEXTURE24);
	glBindTexture(GL_TEXTURE_2D, this->roughDepthMapTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// Attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, this->roughDepthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->roughDepthMapTex, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// High resolution shadows ---------------------------------------------------------------------
	// Configure depth map FBO
	glGenFramebuffers(1, &this->depthMapFBO);
	
	// Create depth texture
	glGenTextures(1, &this->depthMapTex);
	glActiveTexture(GL_TEXTURE25);
	glBindTexture(GL_TEXTURE_2D, this->depthMapTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, this->shadowWidth, this->shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// Attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, this->depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depthMapTex, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// --------------------------------------------------------------------------------------------
	// QUAD FOR RENDERING FINAL PASS
	// --------------------------------------------------------------------------------------------
	// Create quad VAO for final default framebuffer image render
	float quadVerts[] = {
		// Position   // Tex Coords
		-1.0f,  1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,

		-1.0f,  1.0f, 0.0f, 1.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,
	};

	glGenVertexArrays(1, &this->quadVAO);
	glGenBuffers(1, &this->quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), &quadVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

RenderingSystem::~RenderingSystem() { }

unsigned int RenderingSystem::getShaderId()
{
	return this->shader.getId();
}

Shader& RenderingSystem::getShader()
{
	return this->shader;
}

void RenderingSystem::loadModels()
{
	//-----------------------------------------------------------------------------------
	// Player models
	//-----------------------------------------------------------------------------------
	std::string breadbusPath = "resources/models/breadbus/breadbus.obj";
	std::string pancakebusPath = "resources/models/breadbus/pancakebus.obj";
	std::string baguettebusPath = "resources/models/breadbus/baguettebus.obj";
	std::string cakebusPath = "resources/models/breadbus/cakebus.obj";
	
	// Player 1
	this->models.emplace_back(Model(&breadbusPath[0]));
	g_scene.getEntity("player1")->attachComponent(&(this->models[0]), "model");
	
	// Player 2
	this->models.emplace_back(Model(&pancakebusPath[0]));
	g_scene.getEntity("player2")->attachComponent(&(this->models[1]), "model");
	
	// Player 3
	this->models.emplace_back(Model(&baguettebusPath[0]));
	g_scene.getEntity("player3")->attachComponent(&(this->models[2]), "model");
	
	// Player 4
	this->models.emplace_back(Model(&cakebusPath[0]));
	g_scene.getEntity("player4")->attachComponent(&(this->models[3]), "model");

	//-----------------------------------------------------------------------------------
	// Environment models
	//-----------------------------------------------------------------------------------
	// Ground
	std::string groundPath = "resources/models/kitchen/kitchen.obj";
	this->models.emplace_back(Model(&groundPath[0])); 
	g_scene.getEntity("countertop")->attachComponent(&(this->models[4]), "model");

	//-----------------------------------------------------------------------------------
	// Ingredient models
	//-----------------------------------------------------------------------------------
	// Cheese
	std::string cheesePath = "resources/models/ingredients/cheese.obj";
	this->models.emplace_back(Model(&cheesePath[0]));
	g_scene.getEntity("cheese")->attachComponent(&(this->models[5]), "model");

	// Sausage
	std::string sausagePath = "resources/models/ingredients/sausage.obj";
	this->models.emplace_back(Model(&sausagePath[0]));
	g_scene.getEntity("sausage")->attachComponent(&(this->models[6]), "model");

	// Tomato
	std::string tomatoPath = "resources/models/ingredients/tomato.obj";
	this->models.emplace_back(Model(&tomatoPath[0]));
	g_scene.getEntity("tomato")->attachComponent(&(this->models[7]), "model");

	// Dough
	std::string doughPath = "resources/models/ingredients/dough.obj";
	this->models.emplace_back(Model(&doughPath[0]));
	g_scene.getEntity("dough")->attachComponent(&(this->models[8]), "model");

	// Carrot
	std::string carrotPath = "resources/models/ingredients/carrot.obj";
	this->models.emplace_back(Model(&carrotPath[0]));
	g_scene.getEntity("carrot")->attachComponent(&(this->models[9]), "model");

	// Lettuce
	std::string lettucePath = "resources/models/ingredients/lettuce.obj";
	this->models.emplace_back(Model(&lettucePath[0]));
	g_scene.getEntity("lettuce")->attachComponent(&(this->models[10]), "model");

	// Parsnip
	std::string parsnipPath = "resources/models/ingredients/parsnip.obj";
	this->models.emplace_back(Model(&parsnipPath[0]));
	g_scene.getEntity("parsnip")->attachComponent(&(this->models[11]), "model");

	// Rice
	std::string ricePath = "resources/models/ingredients/rice.obj";
	this->models.emplace_back(Model(&ricePath[0]));
	g_scene.getEntity("rice")->attachComponent(&(this->models[12]), "model");

	// Egg
	std::string eggPath = "resources/models/ingredients/egg.obj";
	this->models.emplace_back(Model(&eggPath[0]));
	g_scene.getEntity("egg")->attachComponent(&(this->models[13]), "model");

	// Chicken
	std::string chickenPath = "resources/models/ingredients/chicken.obj";
	this->models.emplace_back(Model(&chickenPath[0]));
	g_scene.getEntity("chicken")->attachComponent(&(this->models[14]), "model");

	// Peas
	std::string peasPath = "resources/models/ingredients/peas.obj";
	this->models.emplace_back(Model(&peasPath[0]));
	g_scene.getEntity("peas")->attachComponent(&(this->models[15]), "model");

	// Soupbase
	std::string soupbasePath = "resources/models/ingredients/soupbase.obj";
	this->models.emplace_back(Model(&soupbasePath[0]));
	g_scene.getEntity("soupbase")->attachComponent(&(this->models[16]), "model");

	// Pumpkin
	std::string pumpkinPath = "resources/models/ingredients/pumpkin.obj";
	this->models.emplace_back(Model(&pumpkinPath[0]));
	g_scene.getEntity("pumpkin")->attachComponent(&(this->models[17]), "model");

	//-----------------------------------------------------------------------------------
	// Debug models
	//-----------------------------------------------------------------------------------
	std::string testPath = "resources/models/ball/ball.obj";
	this->models.emplace_back(Model(&testPath[0]));
	g_scene.getEntity("test")->attachComponent(&(this->models[18]), "model");
}

void RenderingSystem::setupCameras(Transform* player1Transform)
{
	// Get view matrix from Camera and update the Shader
	glm::mat4 view = g_scene.camera.getViewMatrix(player1Transform);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	// Tell shader about camera position
	unsigned int viewPosLoc = glGetUniformLocation(getShaderId(), "viewPos");
	glm::vec3 cameraPos = g_scene.camera.position;
	glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);

	// Projection matrix will be handled by the Camera class in the future
	glm::mat4 proj = glm::mat4(1.0f);
	float screenWidth = g_systems.width;
	float screenHeight = g_systems.height;

	proj = glm::perspective(glm::radians(g_scene.camera.getPerspective()), screenWidth / screenHeight, 0.1f, 1000.0f);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
}

// Orthographic projection for the depth map that shows the entire level layout for rough shadows at a distance
void RenderingSystem::updateRoughOrtho()
{
	this->ort.left = 281.0f;
	this->ort.right = -279.0f;
	this->ort.bottom = -205.0f;
	this->ort.top = 197.0f;
}

// Moves the orthographic projection used for the depth map so that it follows the player for high res shadows
void RenderingSystem::updateOrtho()
{
	glm::vec3 p1Pos = g_scene.getEntity("player1")->getTransform()->position;

	this->ort.left = p1Pos.x + 100.0f;
	this->ort.right = p1Pos.x - 100.0f;
	this->ort.bottom = -p1Pos.z - 100.0f;
	this->ort.top = -p1Pos.z + 100.0f;
}

void RenderingSystem::update()
{
	// Rough shadows -----------------------------------------------------------------
	// Render roughDepthMap to texture (from light's perspective)
	glm::mat4 lightProjection, lightView, roughLightSpaceMatrix, lightSpaceMatrix;
	updateRoughOrtho();
	lightProjection = glm::ortho(ort.left, ort.right, ort.bottom, ort.top, ort.nearPlane, ort.farPlane);
	lightView = glm::lookAt(this->lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	roughLightSpaceMatrix = lightProjection * lightView;

	depthShader.use();
	depthShader.setMat4("lightSpaceMatrix", roughLightSpaceMatrix);

	glViewport(0, 0, 1024, 1024);
	glBindFramebuffer(GL_FRAMEBUFFER, this->roughDepthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);
	renderShadowMap();
	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// High res shadows --------------------------------------------------------------
	// Render depthMap to texture (from light's perspective)
	updateOrtho();
	lightProjection = glm::ortho(ort.left, ort.right, ort.bottom, ort.top, ort.nearPlane, ort.farPlane);
	lightView = glm::lookAt(this->lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	lightSpaceMatrix = lightProjection * lightView;

	depthShader.use();
	depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

	glViewport(0, 0, this->shadowWidth, this->shadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, this->depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT); // Switch to front-face culling to reduce peter-panning of shadows
	renderShadowMap();
	glCullFace(GL_BACK); // Switch back to back-face culling for regular rendering
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Regular render pass -------------------------------------------------------------
	// Debug code for rendering the depthMap to viewport
	if (g_systems.renderDebug)
		renderDebugShadowMap();
	else
	{
		// Render scene as normal using the generated depth/shadow map
		this->shader.use();
		this->shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		this->shader.setMat4("roughLightSpaceMatrix", roughLightSpaceMatrix);
		this->shader.setVec3("playerPos", g_scene.getEntity("player1")->getTransform()->position);
		this->shader.setMat4("playerModelMatrix", g_scene.getEntity("player1")->getTransform()->getModelMatrix());
		renderScene();
	}
}

void RenderingSystem::renderDebugShadowMap()
{
	// Debug code for rendering the depthMap to viewport
	glViewport(0, 0, g_systems.width, g_systems.height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	this->depthShader.use();
	glActiveTexture(GL_TEXTURE25);
	glBindTexture(GL_TEXTURE_2D, this->depthMapTex);
	glUniform1i(glGetUniformLocation(this->debugShader.getId(), "shadowMap"), 25);
	renderTexturedQuad();
	glBindTexture(GL_TEXTURE_2D, 0);
}

Model* RenderingSystem::getKitchenModel()
{
	return g_scene.getEntity("countertop")->getModel();
}

void RenderingSystem::renderScene()
{
	// Reset viewport
	glViewport(0, 0, g_systems.width, g_systems.height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Update camera (MVP matrices)
	Transform* p1Transform = g_scene.getEntity("player1")->getTransform();
	g_scene.camera.updateCameraVectors(p1Transform);
	setupCameras(p1Transform);

	// Bind rough shadow map
	glActiveTexture(GL_TEXTURE24);
	glBindTexture(GL_TEXTURE_2D, this->roughDepthMapTex);
	glUniform1i(glGetUniformLocation(this->shader.getId(), "roughShadowMap"), 24);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE, 0);

	// Bind high res shadow map
	glActiveTexture(GL_TEXTURE25);
	glBindTexture(GL_TEXTURE_2D, this->depthMapTex);
	glUniform1i(glGetUniformLocation(this->shader.getId(), "shadowMap"), 25);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE, 0);

	glUniform1i(texLoc, 1);

	// Set shadow bias uniforms
	
	shader.setFloat("maxBias", this->maxBias);
	shader.setFloat("minBias", this->minBias);
	shader.setFloat("maxRoughBias", this->maxRoughBias);
	shader.setFloat("minRoughBias", this->minRoughBias);

	// Iterate through all the models in the scene and render them at their new transforms
	for (int i = 0; i < models.size(); i++)
	{
		Transform* ownerTransform = models[i].owner->getTransform();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(ownerTransform->getModelMatrix()));

		if (i < 4) // Use material info for player models
		{
			glUniform1i(texLoc, 0);
			models[i].draw(this->shader);
		}
		else if (i >= 4) // Use texture images for everything else
		{
			//ownerTransform->update();

			glUniform1i(texLoc, 1);
			models[i].draw(this->shader);
		}
	}
}

void RenderingSystem::renderShadowMap()
{	
	// Get model unfirom location in shader
	unsigned int modelLoc = glGetUniformLocation(this->depthShader.getId(), "model");

	// Render scene to depthMapFBO
	for (auto it = this->models.begin(); it < this->models.end(); it++)
	{
		// Update model matrix
		Transform* ownerTransform = it->owner->getTransform();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(ownerTransform->getModelMatrix()));

		it->drawDepthMap(this->depthShader);
	}
}

void RenderingSystem::renderTexturedQuad()
{
	// Draw to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, g_systems.width, g_systems.height);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render scene to viewport by applying depthMap as texture to 2D quad
	this->debugShader.use();
	this->debugShader.setInt("depthMap", 25);
	glBindVertexArray(this->quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}