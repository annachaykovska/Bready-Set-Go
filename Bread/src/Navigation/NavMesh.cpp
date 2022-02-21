#include "NavMesh.h"

NavMesh::NavMesh()
{
	Node *n1, *n2, *n3, *n4, *n5, *n6, *n7, *n8, *n9, *n10, *n11, *n12, *n13, *n14, *n15, *n16, *n17, *n18, *n19, *n20;

	n1 = new Node(vert(0.0f, 0.0f, 0.0f));
	n2 = new Node(vert(20.0f, 0.0f, 0.0f));
	n3 = new Node(vert(40.0f, 0.0f, 0.0f));
	n4 = new Node(vert(60.0f, 0.0f, 0.0f));
	n5 = new Node(vert(80.0f, 0.0f, 0.0f));

	n6 = new Node(vert(0.0f, 0.0f, -20.0f));
	n7 = new Node(vert(20.0f, 0.0f, -20.0f));
	n8 = new Node(vert(40.0f, 0.0f, -20.0f));
	n9 = new Node(vert(60.0f, 0.0f, -20.0f));
	n10 = new Node(vert(80.0f, 0.0f, -20.0f));

	n11 = new Node(vert(0.0f, 0.0f, -40.0f));
	n12 = new Node(vert(20.0f, 0.0f, -40.0f));
	n13 = new Node(vert(40.0f, 0.0f, -40.0f));
	n14 = new Node(vert(60.0f, 0.0f, -40.0f));

	n15 = new Node(vert(0.0f, 0.0f, -60.0f));
	n16 = new Node(vert(20.0f, 0.0f, -60.0f));
	n17 = new Node(vert(40.0f, 0.0f, -60.0f));
	n18 = new Node(vert(60.0f, 0.0f, -60.0f));

	n19 = new Node(vert(40.0f, 0.0f, -80.0f));
	n20 = new Node(vert(60.0f, 0.0f, -80.0f));

	MeshSegment* A = new MeshSegment(n2, n6, n1);
	MeshSegment* B = new MeshSegment(n6, n2, n7);
	MeshSegment* C = new MeshSegment(n2, n7, n8);
	MeshSegment* D = new MeshSegment(n2, n3, n8);
	MeshSegment* E = new MeshSegment(n8, n3, n4);
	MeshSegment* F = new MeshSegment(n8, n4, n9);
	MeshSegment* G = new MeshSegment(n4, n9, n5);
	MeshSegment* H = new MeshSegment(n9, n5, n10);
	MeshSegment* I = new MeshSegment(n11, n6, n7);
	MeshSegment* J = new MeshSegment(n11, n7, n12);
	MeshSegment* K = new MeshSegment(n13, n8, n14);
	MeshSegment* L = new MeshSegment(n8, n14, n9);
	MeshSegment* M = new MeshSegment(n15, n11, n12);
	MeshSegment* N = new MeshSegment(n15, n12, n16);
	MeshSegment* O = new MeshSegment(n12, n16, n17);
	MeshSegment* P = new MeshSegment(n12, n17, n13);
	MeshSegment* Q = new MeshSegment(n17, n13, n14);
	MeshSegment* R = new MeshSegment(n17, n14, n18);
	MeshSegment* S = new MeshSegment(n19, n17, n20);
	MeshSegment* T = new MeshSegment(n17, n20, n18);

	A->id_ = 'A';
	B->id_ = 'B';
	C->id_ = 'C';
	D->id_ = 'D';
	E->id_ = 'E';
	F->id_ = 'F';
	G->id_ = 'G';
	H->id_ = 'H';
	I->id_ = 'I';
	J->id_ = 'J';
	K->id_ = 'K';
	L->id_ = 'L';
	M->id_ = 'M';
	N->id_ = 'N';
	O->id_ = 'O';
	P->id_ = 'P';
	Q->id_ = 'Q';
	R->id_ = 'R';
	S->id_ = 'S';
	T->id_ = 'T';

	mesh_.push_back(A);
	mesh_.push_back(B);
	mesh_.push_back(C);
	mesh_.push_back(D);
	mesh_.push_back(E);
	mesh_.push_back(F);
	mesh_.push_back(G);
	mesh_.push_back(H);
	mesh_.push_back(I);
	mesh_.push_back(J);
	mesh_.push_back(K);
	mesh_.push_back(L);
	mesh_.push_back(M);
	mesh_.push_back(N);
	mesh_.push_back(O);
	mesh_.push_back(P);
	mesh_.push_back(Q);
	mesh_.push_back(R);
	mesh_.push_back(S);
	mesh_.push_back(T);

	setupVisualizer();
	connectMesh();
}

void NavMesh::setupVisualizer()
{
	int index = 0;
	for (MeshSegment* seg : mesh_)
	{
		Vertex a, b, c;
		a.position = seg->v0_->position_;
		b.position = seg->v1_->position_;
		c.position = seg->v2_->position_;
		a.normal = seg->normal_;
		b.normal = seg->normal_;
		c.normal = seg->normal_;
		visualizer_.vertices.push_back(a);
		visualizer_.vertices.push_back(b);
		visualizer_.vertices.push_back(c);
		visualizer_.indices.push_back(index);
		visualizer_.indices.push_back(index + 1);
		visualizer_.indices.push_back(index + 2);
		index += 3;
	}

	visualizerTransform_.position = glm::vec3(0, 0, 0);
	visualizerTransform_.rotation = glm::vec3(0, 0, 0);
	visualizerTransform_.scale = glm::vec3(1, 1, 1);

	visualizer_.setWireframe(true);
	visualizer_.setupMesh();
}

void NavMesh::connectMesh()
{
	for (int i = 0; i < mesh_.size(); i++)
	{
		for (int j = 0; j < mesh_.size(); j++)
		{
			int sharesNodes = 0;
			std::vector<Node*> sharedNodes;
			if (mesh_.at(i) != mesh_.at(j))
			{
				if (hasMatchingNode(mesh_.at(i)->v0_, mesh_.at(j)))
				{
					sharesNodes++;
					sharedNodes.push_back(mesh_.at(i)->v0_);
				}
				if (hasMatchingNode(mesh_.at(i)->v1_, mesh_.at(j)))
				{
					sharesNodes++;
					sharedNodes.push_back(mesh_.at(i)->v1_);
				}
				if (hasMatchingNode(mesh_.at(i)->v2_, mesh_.at(j)))
				{
					sharesNodes++;
					sharedNodes.push_back(mesh_.at(i)->v2_);
				}
				
				if (!alreadyContainsNeighbor(mesh_.at(i), mesh_.at(j)) && sharesNodes == 2)
				{
					std::cout << mesh_.at(i)->id_ << " " << mesh_.at(j)->id_ << std::endl;
					mesh_.at(i)->neighbors.push_back(std::pair <Node*, MeshSegment*> (sharedNodes.at(0), mesh_.at(j)));

					mesh_.at(i)->neighbors.push_back(std::pair <Node*, MeshSegment*> (sharedNodes.at(1), mesh_.at(j)));
				}
			}
		}
	}
}

Mesh NavMesh::getWireframe()
{
	return visualizer_;
}

Transform NavMesh::getWireframeTransform()
{
	return visualizerTransform_;
}

std::vector<NavMesh::MeshSegment*> NavMesh::getSegments()
{
	return mesh_;
}

bool NavMesh::hasMatchingNode(Node* node, MeshSegment* segment)
{
	if (node == segment->v0_ || node == segment->v1_ || node == segment->v2_)
	{
		return true;
	}
	return false;
}

bool NavMesh::alreadyContainsNeighbor(MeshSegment* segment, MeshSegment* neighbor)
{
	for (std::pair<Node*, MeshSegment*> neighborSeg : segment->neighbors)
	{
		if (neighborSeg.second == neighbor)
		{
			return true;
		}
	}
	return false;
}