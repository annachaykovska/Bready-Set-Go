#include <glad/glad.h>

#include "VertexArray.h"

// VAO - stores vertex attribute data so it can be reused
VertexArray::VertexArray()
{
	glGenVertexArrays(1, &arrayID);
	bind();
}

void VertexArray::bind()
{
	glBindVertexArray(arrayID);
}