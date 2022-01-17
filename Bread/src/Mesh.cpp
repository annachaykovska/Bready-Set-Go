#include "Mesh.h"
#include <glad/glad.h>

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
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

	glBindVertexArray(0);
}

/// <summary>
/// Renders the mesh to the screen using the provided Shader program.
/// </summary>
/// <param name="shader"> The desired shader. </param>
void Mesh::draw(Shader &shader)
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;

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

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}