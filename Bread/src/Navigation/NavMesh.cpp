#include "NavMesh.h"

NavMesh::NavMesh()
{
	vert n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28;

	n1 = vert(-100.0f, 5.0f, 100.0f);
	n2 = vert(-80.0f, 5.0f, 100.0f);
	n3 = vert(-60.0f, 5.0f, 100.0f);
	n4 = vert(-40.0f, 5.0f, 100.0f);
	n5 = vert(-20.0f, 5.0f, 100.0f);

	n6 = vert(-100.0f, 5.0f, 80.0f);
	n7 = vert(-80.0f, 5.0f, 80.0f);
	n8 = vert(-60.0f, 5.0f, 80.0f);
	n9 = vert(-40.0f, 5.0f, 80.0f);
	n10 = vert(-20.0f, 5.0f, 80.0f);

	n11 = vert(-100.0f, 5.0f, 60.0f);
	n12 = vert(-80.0f, 5.0f, 60.0f);
	n13 = vert(-60.0f, 5.0f, 60.0f);
	n14 = vert(-40.0f, 5.0f, 60.0f);

	n15 = vert(-100.0f, 5.0f, 40.0f);
	n16 = vert(-80.0f, 5.0f, 40.0f);
	n17 = vert(-60.0f, 5.0f, 40.0f);
	n18 = vert(-40.0f, 5.0f, 40.0f);

	n19 = vert(-60.0f, 5.0f, 20.0f);
	n20 = vert(-40.0f, 5.0f, 20.0f);
	n21 = vert(-60.0f, 5.0f, 0.0f);
	n22 = vert(-40.0f, 5.0f, 0.0f);

	n23 = vert(-20.0f, 5.0f, 0.0f);
	n24 = vert(-20.0f, 5.0f, 20.0f);
	n25 = vert(0.0f, 5.0f, 0.0f);
	n26 = vert(0.0f, 5.0f, 20.0f);

	n27 = vert(-20.0f, 5.0f, 40.0f);
	n28 = vert(0.0f, 5.0f, 20.0f);

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
	MeshSegment* U = new MeshSegment(n19, n20, n21);
	MeshSegment* V = new MeshSegment(n20, n22, n21);
	MeshSegment* W = new MeshSegment(n20, n23, n22);
	MeshSegment* X = new MeshSegment(n20, n24, n23);
	MeshSegment* Y = new MeshSegment(n24, n26, n23);
	MeshSegment* Z = new MeshSegment(n26, n25, n23);
	MeshSegment* AA = new MeshSegment(n24, n27, n26);
	MeshSegment* AB = new MeshSegment(n27, n28, n26);

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
	U->id_ = 'U';
	V->id_ = 'V';
	W->id_ = 'W';
	X->id_ = 'X';
	Y->id_ = 'Y';
	Z->id_ = 'Z';
	AA->id_ = 'a';
	AB->id_ = 'b';

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
	mesh_.push_back(U);
	mesh_.push_back(V);
	mesh_.push_back(W);
	mesh_.push_back(X);
	mesh_.push_back(Y);
	mesh_.push_back(Z);
	mesh_.push_back(AA);
	mesh_.push_back(AB);

	setupVisualizer();
	connectMesh();
}

void NavMesh::setupVisualizer()
{
	int index = 0;
	for (MeshSegment* seg : mesh_)
	{
		Vertex a, b, c;
		a.position = seg->v0_;
		b.position = seg->v1_;
		c.position = seg->v2_;
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
			std::vector<vert> sharedNodes;
			if (mesh_.at(i) != mesh_.at(j))
			{
				if (hasMatchingVertex(mesh_.at(i)->v0_, mesh_.at(j)))
				{
					sharesNodes++;
					sharedNodes.push_back(mesh_.at(i)->v0_);
				}
				if (hasMatchingVertex(mesh_.at(i)->v1_, mesh_.at(j)))
				{
					sharesNodes++;
					sharedNodes.push_back(mesh_.at(i)->v1_);
				}
				if (hasMatchingVertex(mesh_.at(i)->v2_, mesh_.at(j)))
				{
					sharesNodes++;
					sharedNodes.push_back(mesh_.at(i)->v2_);
				}
				
				if (!alreadyContainsNeighbor(mesh_.at(i), mesh_.at(j)) && sharesNodes == 2)
				{
					std::cout << mesh_.at(i)->id_ << " " << mesh_.at(j)->id_ << std::endl;
					mesh_.at(i)->neighbors.push_back(std::pair <Node*, MeshSegment*> (mesh_.at(j)->meshStep_, mesh_.at(j)));
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

bool NavMesh::hasMatchingVertex(vert node, NavMesh::MeshSegment* segment)
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