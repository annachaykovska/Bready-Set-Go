#pragma once
#include "../Rendering/Mesh.h"
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
		MeshSegment(vert v0, vert v1, vert v2, glm::vec3 normal = vert(0.0f, 1.0f, 0.0f))
		{
			v0_ = v0;
			v1_ = v1;
			v2_ = v2;
			normal_ = normal;
			
			meshStep_ = new Node(vert((v0_.x + v1_.x + v2_.x) / 3.f, (v0_.y + v1_.y + v2_.y) / 3.f, (v0_.z + v1_.z + v2_.z) / 3.f));
		}

		int id_;
		bool traversable_ = true;
		vert v0_;
		vert v1_;
		vert v2_;
		Node* meshStep_;
		glm::vec3 normal_;

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
	bool hasMatchingVertex(vert node, MeshSegment* segment);
	bool alreadyContainsNeighbor(MeshSegment* segment, MeshSegment* neighbor);

	std::vector<MeshSegment*> mesh_;
	Mesh visualizer_;
	Transform visualizerTransform_;
};