#include <vector>

#include <glad/glad.h>

#include "ElementBuffer.h"

ElementBuffer::ElementBuffer()
{
	glGenBuffers(1, &bufferID);
}

void ElementBuffer::bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
}

void ElementBuffer::uploadData(GLsizeiptr size, const void* data, GLenum usage)
{
	bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
}