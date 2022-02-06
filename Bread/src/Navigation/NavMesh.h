#pragma once
#include "../Mesh.h"

struct MeshSegment
{
	int id;
	glm::vec3 v0, v1, v2;
};

class NavMesh
{
public:
	NavMesh();

private:
	Mesh visualizer;
};