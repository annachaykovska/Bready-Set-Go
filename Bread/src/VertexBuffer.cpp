#include <glad/glad.h>

#include "VertexBuffer.h"
#include <iostream>

// VBO - holds the data for delivery from CPU to GPU
VertexBuffer::VertexBuffer(GLint index, GLint size, GLenum dataType) 
{
	glGenBuffers(1, &bufferID);
	bind();
}

// Bind the buffer to GL_ARRAY_BUFFER
void VertexBuffer::bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);
}