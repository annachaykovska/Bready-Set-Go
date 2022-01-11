#pragma once

class VertexBuffer {
public:

	VertexBuffer(GLint index, GLint size, GLenum dataType);

	void bind();
	void cleanup() { glDeleteBuffers(1, &bufferID); }

private:

	unsigned int bufferID;
};
