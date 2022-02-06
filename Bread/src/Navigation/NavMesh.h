#pragma once
#include "../Mesh.h"
#include "../Transform.h"

typedef std::pair<int, int> SegmentVertex;
typedef glm::vec3 vert;

class Node
{
public:
	Node(vert position)
	{
		position_ = position;
	}
	int id_;
	vert position_;
};

class NavMesh
{
public:
	class MeshSegment
	{
	public:
		MeshSegment(Node* v0, Node* v1, Node* v2, vert normal = vert(0.0f, 1.0f, 0.0f))
		{
			v0_ = v0;
			v1_ = v1;
			v2_ = v2;
			normal_ = normal;
		}

		char id_;
		bool traversable_ = true;
		Node* v0_;
		Node* v1_;
		Node* v2_;
		vert normal_;

		std::vector<std::pair<Node*, MeshSegment*>> neighbors;
		int numNeighbors;
	};
	NavMesh();

	void connectMesh();
	Mesh getWireframe();
	Transform getWireframeTransform();
	std::vector<MeshSegment*> getSegments();

private:
	void setupVisualizer();
	bool hasMatchingNode(Node* node, MeshSegment* segment);
	bool alreadyContainsNeighbor(MeshSegment* segment, MeshSegment* neighbor);

	std::vector<MeshSegment*> mesh_;
	Mesh visualizer_;
	Transform visualizerTransform_;
};