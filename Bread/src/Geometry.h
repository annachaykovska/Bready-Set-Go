#pragma once

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "ElementBuffer.h"
#include "Texture.h"

struct CPU_Geometry 
{
	std::vector<glm::vec3> verts;
	std::vector<glm::vec3> colors;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::ivec3> indices;
};

class GPU_Geometry {
public:

	CPU_Geometry geom;
	//Texture texture;

	GPU_Geometry();
	GPU_Geometry(CPU_Geometry& shape);

	void bind();
	void cleanup();
	void upload(CPU_Geometry &shape);
	void draw();

private:

	VertexArray VAO;
	VertexBuffer VBO;
	ElementBuffer EBO;
};