#include "Geometry.h"

GPU_Geometry::GPU_Geometry() :
	geom(), VAO(), VBO(0, 3, GL_FLOAT), EBO()
{}

GPU_Geometry::GPU_Geometry(CPU_Geometry& shape) :
	geom(), VAO(), VBO(0, 3, GL_FLOAT), EBO()
{
	upload(shape);
}

void GPU_Geometry::bind()
{
	VAO.bind();
}

void GPU_Geometry::cleanup()
{
	VAO.cleanup();
	VBO.cleanup();
	EBO.cleanup();
}

void GPU_Geometry::upload(CPU_Geometry &shape)
{
	geom = shape;

	size_t vSize = sizeof(glm::vec3) * geom.verts.size();
	size_t cSize = sizeof(glm::vec3) * geom.colors.size();
	size_t tSize = sizeof(glm::vec2) * geom.texCoords.size();
	size_t totalSize = vSize + cSize + tSize;
	
	VBO.bind();
	glBufferData(GL_ARRAY_BUFFER, totalSize, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, geom.verts.data());
	glBufferSubData(GL_ARRAY_BUFFER, vSize, cSize, geom.colors.data());
	glBufferSubData(GL_ARRAY_BUFFER, vSize + cSize, tSize, geom.texCoords.data());

	size_t iSize = sizeof(glm::ivec3) * geom.indices.size();

	EBO.bind();
	EBO.uploadData(iSize, geom.indices.data(), GL_STATIC_DRAW);
}

void GPU_Geometry::draw()
{
	VBO.bind();
	EBO.bind();
	texture.bind();

	size_t vOffset = sizeof(glm::vec3) * geom.verts.size();
	size_t cOffset = vOffset + sizeof(glm::vec3) * geom.colors.size();

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)vOffset);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)cOffset);

	size_t size = sizeof(glm::ivec3) * geom.indices.size();

	glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
}