#include "NavMesh.h"

NavMesh::NavMesh()
{
	vert n1, n2, n3, n4, n5, n6, n7, n8, n9, n10;
	vert n11, n12, n13, n14, n15, n16, n17, n18, n19, n20;
	vert n21, n22, n23, n24, n25, n26, n27, n28, n29, n30;
	vert n31, n32, n33, n34, n35, n36, n37, n38, n39, n40;
	vert n41, n42, n43, n44;

	//KITCHEN COUNTER
	n1 = vert(90, 2, -145);
	n2 = vert(96, 2, -86);
	n3 = vert(102, 2, 1);
	n4 = vert(92, 2, 56);
	n5 = vert(75, 2, 21);
	n6 = vert(70, 2, -45);
	n7 = vert(41, 2, -147);
	n8 = vert(41, 2, -127);
	n9 = vert(41, 2, -90);
	n10 = vert(41, 2, 1);
	n11 = vert(68, 2, 62);
	n12 = vert(2, 2, 30);
	n13 = vert(5, 2, -52);
	n14 = vert(-27, 2, -90);
	n15 = vert(-27, 2, 1);
	n16 = vert(-55, 2, 32);
	n17 = vert(-55, 2, 65);
	n18 = vert(-69, 2, 31);
	n19 = vert(-71, 2, -20);
	n20 = vert(-87, 2, -51);
	n21 = vert(-100, 2, -80);
	n22 = vert(-50, 2, -117);
	n23 = vert(-70, 2, -150);
	n24 = vert(-96, 2, -120);
	n25 = vert(-100, 2, 1);
	n26 = vert(-90, 2, 31);
	n27 = vert(-120, 2, -75);
	n28 = vert(-120, 2, -34);
	n29 = vert(-210, 2, -30);
	n30 = vert(-210, 2, -70);
	n31 = vert(-260, 2, -35);
	n32 = vert(-225, 2, -75);
	n33 = vert(-225, 2, -113);
	n34 = vert(-260, 2, -140);
	n35 = vert(-230, 2, -155);
	n36 = vert(96, 2, -58);
	n37 = vert(-150, 2, -150);
	n38 = vert(-160, 2, -120);

	// COUNTER RAMPS
	n39 = vert(163, 6, -82);
	n40 = vert(165, 6, -58);
	n41 = vert(-85, 58, 144);
	n42 = vert(-69, 58, 141);
	n43 = vert(92, 6, 167);
	n44 = vert(72, 6, 167);

	// KITCHEN COUNTER
	MeshSegment* m1 = new MeshSegment(n8, n1, n7);
	MeshSegment* m2 = new MeshSegment(n2, n1, n8);
	MeshSegment* m3 = new MeshSegment(n9, n2, n8);
	MeshSegment* m4 = new MeshSegment(n6, n2, n9);
	MeshSegment* m5 = new MeshSegment(n6, n36, n2);

	MeshSegment* m6 = new MeshSegment(n3, n36, n6);
	MeshSegment* m7 = new MeshSegment(n10, n3, n6);
	MeshSegment* m8 = new MeshSegment(n10, n6, n9);
	MeshSegment* m9 = new MeshSegment(n5, n3, n10);
	MeshSegment* m10 = new MeshSegment(n5, n4, n3);

	MeshSegment* m11 = new MeshSegment(n11, n4, n5);
	MeshSegment* m12 = new MeshSegment(n11, n5, n10);
	MeshSegment* m13 = new MeshSegment(n11, n10, n12);
	MeshSegment* m14 = new MeshSegment(n17, n11, n12);
	MeshSegment* m15 = new MeshSegment(n17, n12, n16);

	MeshSegment* m16 = new MeshSegment(n16, n12, n15);
	MeshSegment* m17 = new MeshSegment(n18, n16, n15);
	MeshSegment* m18 = new MeshSegment(n12, n10, n15);
	MeshSegment* m19 = new MeshSegment(n15, n10, n13);
	MeshSegment* m20 = new MeshSegment(n10, n9, n13);

	MeshSegment* m21 = new MeshSegment(n13, n9, n14);
	MeshSegment* m22 = new MeshSegment(n15, n13, n14);
	MeshSegment* m23 = new MeshSegment(n15, n14, n20);
	MeshSegment* m24 = new MeshSegment(n15, n20, n19);
	MeshSegment* m25 = new MeshSegment(n25, n15, n19);

	MeshSegment* m26 = new MeshSegment(n18, n15, n25);
	MeshSegment* m27 = new MeshSegment(n18, n25, n26);
	MeshSegment* m28 = new MeshSegment(n20, n14, n21);
	MeshSegment* m29 = new MeshSegment(n20, n21, n27);
	MeshSegment* m30 = new MeshSegment(n20, n27, n28);

	MeshSegment* m31 = new MeshSegment(n28, n27, n30);
	MeshSegment* m32 = new MeshSegment(n28, n30, n29);
	MeshSegment* m33 = new MeshSegment(n29, n30, n32);
	MeshSegment* m34 = new MeshSegment(n29, n32, n31);
	MeshSegment* m35 = new MeshSegment(n31, n32, n34);

	MeshSegment* m36 = new MeshSegment(n32, n33, n34);
	MeshSegment* m37 = new MeshSegment(n33, n35, n34);
	MeshSegment* m38 = new MeshSegment(n33, n38, n35);
	MeshSegment* m39 = new MeshSegment(n38, n37, n35);
	MeshSegment* m40 = new MeshSegment(n38, n24, n37);

	MeshSegment* m41 = new MeshSegment(n24, n23, n37);
	MeshSegment* m42 = new MeshSegment(n24, n22, n23);
	MeshSegment* m43 = new MeshSegment(n21, n22, n24);
	MeshSegment* m44 = new MeshSegment(n21, n14, n22);

	// COUNTER RAMPS
	MeshSegment* m45 = new MeshSegment(n36, n40, n2);
	MeshSegment* m46 = new MeshSegment(n40, n39, n2);
	MeshSegment* m47 = new MeshSegment(n42, n18, n26);
	MeshSegment* m48 = new MeshSegment(n41, n42, n26);
	MeshSegment* m49 = new MeshSegment(n43, n4, n11);

	MeshSegment* m50 = new MeshSegment(n44, n43, n11);

	//// KITCHEN COUNTER
	//m1->id_ = 1;
	//m2->id_ = 2;
	//m3->id_ = 3;
	//m4->id_ = 4;
	//m5->id_ = 5;

	//m6->id_ = 6;
	//m7->id_ = 7;
	//m8->id_ = 8;
	//m9->id_ = 9;
	//m10->id_ = 10;

	//m11->id_ = 11;
	//m12->id_ = 12;
	//m13->id_ = 13;
	//m14->id_ = 14;
	//m15->id_ = 15;

	//m16->id_ = 16;
	//m17->id_ = 17;
	//m18->id_ = 18;
	//m19->id_ = 19;
	//m20->id_ = 20;

	//m21->id_ = 21;
	//m22->id_ = 22;
	//m23->id_ = 23;
	//m24->id_ = 24;
	//m25->id_ = 25;

	//m26->id_ = 26;
	//m27->id_ = 27;
	//m28->id_ = 28;
	//m29->id_ = 29;
	//m30->id_ = 30;

	//m31->id_ = 31;
	//m32->id_ = 32;
	//m33->id_ = 33;
	//m34->id_ = 34;
	//m35->id_ = 35;

	//m36->id_ = 36;
	//m37->id_ = 37;
	//m38->id_ = 38;
	//m39->id_ = 39;
	//m40->id_ = 40;

	//m41->id_ = 41;
	//m42->id_ = 42;
	//m43->id_ = 43;
	//m44->id_ = 44;

	//// COUNTER RAMPS
	//m45->id_ = 45;
	//m46->id_ = 

	// KITCHEN COUNTER
	mesh_.push_back(m1);
	mesh_.push_back(m2);
	mesh_.push_back(m3);
	mesh_.push_back(m4);
	mesh_.push_back(m5);

	mesh_.push_back(m6);
	mesh_.push_back(m7);
	mesh_.push_back(m8);
	mesh_.push_back(m9);
	mesh_.push_back(m10);

	mesh_.push_back(m11);
	mesh_.push_back(m12);
	mesh_.push_back(m13);
	mesh_.push_back(m14);
	mesh_.push_back(m15);

	mesh_.push_back(m16);
	mesh_.push_back(m17);
	mesh_.push_back(m18);
	mesh_.push_back(m19);
	mesh_.push_back(m20);

	mesh_.push_back(m21);
	mesh_.push_back(m22);
	mesh_.push_back(m23);
	mesh_.push_back(m24);
	mesh_.push_back(m25);
	
	mesh_.push_back(m26);
	mesh_.push_back(m27);
	mesh_.push_back(m28);
	mesh_.push_back(m29);
	mesh_.push_back(m30);

	mesh_.push_back(m31);
	mesh_.push_back(m32);
	mesh_.push_back(m33);
	mesh_.push_back(m34);
	mesh_.push_back(m35);

	mesh_.push_back(m36);
	mesh_.push_back(m37);
	mesh_.push_back(m38);
	mesh_.push_back(m39);
	mesh_.push_back(m40);

	mesh_.push_back(m41);
	mesh_.push_back(m42);
	mesh_.push_back(m43);
	mesh_.push_back(m44);

	// COUNTER RAMPS
	mesh_.push_back(m45);
	mesh_.push_back(m46);
	mesh_.push_back(m47);
	mesh_.push_back(m48);
	mesh_.push_back(m49);

	mesh_.push_back(m50);

	for (int i = 1; i <= mesh_.size(); i++)
	{
		mesh_.at(i-1)->id_ = i;
	}

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
					//std::cout << mesh_.at(i)->id_ << " " << mesh_.at(j)->id_ << std::endl;
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