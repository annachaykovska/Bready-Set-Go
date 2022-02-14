#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Material.h"

struct Vertex
{
	glm::vec3 position;  // Position vector of the vertex
	glm::vec3 normal;	 // Normal vector of the vertex 
	glm::vec2 texCoords; // The texture coordinate of the vertex
};

struct Texture
{
	unsigned int id;  // The identifier of the texture image
	std::string type; // The type of texture (i.e. diffuse, specular, etc.)
	std::string path; // The path to the texture's image file
};

class Mesh
{
public:

	std::vector<Vertex> vertices;	   // List of vertices in the mesh
	std::vector<unsigned int> indices; // List of indices of faces in the mesh
	std::vector<Texture> textures;	   // List of textures used by the mesh
	glm::vec3 color;

	Material material;

	Mesh();
	Mesh(const Mesh& rhs);
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void setupMesh();		   // Initialization
	void setWireframe(bool drawWireframe);
	void draw(Shader& shader); // Render mesh

private:

	unsigned int VAO, VBO, EBO; // Rendering data
	bool wireframe;
	//void setupMesh();			// Initialization

};