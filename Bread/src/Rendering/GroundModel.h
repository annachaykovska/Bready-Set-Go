#pragma once
/*
#include <stbi/stb_image.h>

#include "Model.h"

class GroundModel
{
public:

	// Make ground plane
	static Model createGround()
	{
		Mesh ground;

		// Add vertices to mesh
		Vertex g0, g1, g2, g3;
		g0.position = glm::vec3(-1.0f, 0.0f, -1.0f);
		g0.normal = glm::vec3(0.0f, 1.0f, 0.0f);
		g0.texCoords = glm::vec2(0.0f, 0.0f);
		ground.vertices.push_back(g0);
		g1.position = glm::vec3(1.0f, 0.0f, -1.0f);
		g1.normal = glm::vec3(0.0f, 1.0f, 0.0f);
		g1.texCoords = glm::vec2(1.0f, 0.0f);
		ground.vertices.push_back(g1);
		g2.position = glm::vec3(1.0f, 0.0f, 1.0f);
		g2.normal = glm::vec3(0.0f, 1.0f, 0.0f);
		g2.texCoords = glm::vec2(1.0f, 1.0f);
		ground.vertices.push_back(g2);
		g3.position = glm::vec3(-1.0f, 0.0f, 1.0f);
		g3.normal = glm::vec3(0.0f, 1.0f, 0.0f);
		g3.texCoords = glm::vec2(0.0f, 1.0f);
		ground.vertices.push_back(g3);

		// Add vertex indices for triangles to mesh
		ground.indices.push_back(0);
		ground.indices.push_back(1);
		ground.indices.push_back(2);
		ground.indices.push_back(0);
		ground.indices.push_back(2);
		ground.indices.push_back(3);
		
		// Create and load the wood floor texture
		unsigned int textureId;
		glGenTextures(1, &textureId);
		int width, height, nrChannels;
		unsigned char* data = stbi_load("resources/textures/diffuse.jpg", &width, &height, &nrChannels, 0);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);

		// Add texture to mesh
		Texture groundTexture;
		groundTexture.id = textureId;
		ground.textures.push_back(groundTexture);
		ground.setupMesh();

		Model groundModel(ground);

		return groundModel;
	}
};*/