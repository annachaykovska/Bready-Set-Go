#pragma once

class VertexArray {
public:

	VertexArray();
	void bind();
	void cleanup() { glDeleteVertexArrays(1, &arrayID); }

private:

	unsigned int arrayID;
};