#pragma once
#include "NavMesh.h"

typedef std::pair<Node*, NavMesh::MeshSegment*> pathSegment;
typedef glm::vec3 position;

class PathStep
{
public:
	PathStep() {}

	pathSegment segment;

	double fCost = std::numeric_limits<double>::infinity();
	double gCost = 0;

	PathStep* parent = nullptr;
};

class PathFinder
{
public:
	PathFinder(NavMesh& navMesh);

	std::vector<position> findPath(position start, position end);
	void printPath(PathStep* finalStep);

private:
	void updateTraversablePathSteps();
	NavMesh::MeshSegment* findStartingSegment(position start);

	PathStep* lowestFCost();
	float calculateAndUpdateFCost(position end, PathStep* current, PathStep* neighbor);
	int indexOfPathStep(PathStep* step);
	bool stepContainsTarget(position target, PathStep* step);

	NavMesh& navMesh_;
	std::vector<PathStep*> traversablePathSteps_;

	std::vector<PathStep*> open_;
	std::vector<PathStep*> closed_;
};