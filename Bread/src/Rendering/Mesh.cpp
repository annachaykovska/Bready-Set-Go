#include <iostream>

#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

#include "Mesh.h"
#include "../Scene/Scene.h"
#include "../Scene/Entity.h"
#include "../SystemManager.h"
#include "../Rendering/RenderingSystem.h"

extern Scene g_scene;
extern SystemManager g_systems;

/// <summary>
/// Initializes a Mesh object with the provided lists of vertices, indices, and textures
/// </summary>
/// <param name="vertices"> A vector of vertices. </param>
/// <param name="indices"> A vector of indices. </param>
/// <param name="textures"> A vector of Textures. </param>
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	setupMesh();
}

Mesh::Mesh() {}

Mesh::Mesh(const Mesh& rhs)
{
	this->vertices = rhs.vertices;
	this->indices = rhs.indices;
	this->textures = rhs.textures;
	this->material = rhs.material;
	
	setupMesh();
}

/// <summary>
/// Initializes the vertex array object, vertex buffer object, and element buffer object needed
/// to render this mesh. Then, loads the appropriate vertices, indices, and texture coordinates
/// into the VBO and EBO for rendering and defines them in the VAO.
/// </summary>
void Mesh::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Load the Vertex data (vertices, normals, and texture coordinates) into the VBO
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	// Load the indice data into the EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// Define the attributes (vertices, normals, and texture coordinates) for the VAO
	glEnableVertexAttribArray(0); // (layout = 0) positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1); // (layout = 1) normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2); // (layout = 2) texture coordinates
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

	glBindVertexArray(0); // Unbind VAO
}

void Mesh::setWireframe(bool drawWireFrame)
{
	wireframe = drawWireFrame;
}

/// <summary>
/// Renders the mesh to the screen using the provided Shader program.
/// </summary>
/// <param name="shader"> The desired shader. </param>
void Mesh::draw(Shader &shader)
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;

	glActiveTexture(0);
	glBindTexture(GL_TEXTURE, 0);

	if (textures.size() > 0)
	{
		// Loop through each texture in the mesh to determine its type (diffuse or specular)
		// then construct a uniform name 
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);

			std::string number;
			std::string name = textures[i].type;

			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++);

			shader.setFloat((name + number).c_str(), i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		glActiveTexture(GL_TEXTURE0);
	}

	//glActiveTexture(GL_TEXTURE25);
	//glBindTexture(GL_TEXTURE_2D, g_systems.render->depthMapTex);

	// Material uniforms
	unsigned int ambientLoc = glGetUniformLocation(shader.getId(), "material.ambient");
	glUniform3f(ambientLoc, material.ambient.r, material.ambient.g, material.ambient.b);

	unsigned int diffuseLoc = glGetUniformLocation(shader.getId(), "material.diffuse");
	glUniform3f(diffuseLoc, material.diffuse.r, material.diffuse.g, material.diffuse.b);

	unsigned int specularLoc = glGetUniformLocation(shader.getId(), "material.specular");
	glUniform3f(specularLoc, material.specular.r, material.specular.g, material.specular.b);

	unsigned int shinyLoc = glGetUniformLocation(shader.getId(), "material.shininess");
	glUniform1f(shinyLoc, this->material.shininess);

	unsigned int opaqueLoc = glGetUniformLocation(shader.getId(), "material.opacity");
	glUniform1f(opaqueLoc, this->material.opacity);

	// Light uniforms
	Transform* t = g_scene.getEntity("test")->getTransform();

	unsigned int lightPosLoc = glGetUniformLocation(shader.getId(), "light.position");
	glm::vec3 lightPos = g_systems.render->lightPos;
	glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);

	unsigned int lightColorLoc = glGetUniformLocation(shader.getId(), "light.color");
	glUniform3f(lightColorLoc, 0.9529f, 0.9529f, 0.7765f);

	unsigned int lightDirLoc = glGetUniformLocation(shader.getId(), "light.direction");
	glm::vec3 lightDir = g_systems.render->lightDir;
	glUniform3f(lightDirLoc, lightDir.x, lightDir.y, lightDir.z);

	unsigned int aIntensityLoc = glGetUniformLocation(shader.getId(), "light.ambient");
	glUniform3f(aIntensityLoc, 0.4f, 0.4f, 0.4f);

	unsigned int dIntensityLoc = glGetUniformLocation(shader.getId(), "light.diffuse");
	glUniform3f(dIntensityLoc, 1.0f, 1.0f, 1.0f);

	unsigned int sIntensityLoc = glGetUniformLocation(shader.getId(), "light.specular");
	glUniform3f(sIntensityLoc, 1.0f, 1.0f, 1.0f);

	unsigned int constantLoc = glGetUniformLocation(shader.getId(), "light.constant");
	glUniform1f(constantLoc, 1.0f);

	unsigned int linearLoc = glGetUniformLocation(shader.getId(), "light.linear");
	glUniform1f(linearLoc, 0.09f);

	unsigned int quadraticLoc = glGetUniformLocation(shader.getId(), "light.quadratic");
	glUniform1f(quadraticLoc, 0.032f);

	glBindVertexArray(VAO);

	if (wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::drawDepthMap(Shader& shader)
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}