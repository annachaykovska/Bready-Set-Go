#pragma once

class ElementBuffer {
public:

	ElementBuffer();

	void bind();
	void cleanup() { glDeleteBuffers(1, &bufferID); }
	void uploadData(GLsizeiptr size, const void* data, GLenum usage);

private:

	unsigned int bufferID;
};