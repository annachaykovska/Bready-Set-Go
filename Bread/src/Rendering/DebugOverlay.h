#pragma once
#include <unordered_map>
#include <iostream>
#include "Mesh.h"

typedef std::pair<Mesh, bool> toggleDebugView;

class DebugOverlay
{
public:
	DebugOverlay();

	void addDebugMesh(Mesh mesh, bool show);
	std::vector<toggleDebugView> debugMeshes();

private:
	std::vector<toggleDebugView> meshes;
};