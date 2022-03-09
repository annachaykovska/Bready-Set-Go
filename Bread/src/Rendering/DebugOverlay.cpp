#include "DebugOverlay.h"

DebugOverlay::DebugOverlay()
{
}

void DebugOverlay::addDebugMesh(Mesh mesh, bool show)
{
	meshes.push_back(toggleDebugView(mesh, show));
}

std::vector<toggleDebugView> DebugOverlay::debugMeshes()
{
	return meshes;
}

