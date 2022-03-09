#include "NavMesh.h"

NavMesh::NavMesh()
{
	vert n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15, n16;
	vert n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28, n29, n30, n31, n32;
	vert n33, n34, n35, n36, n37, n38, n39, n40, n41, n42, n43, n44, n45, n46, n47, n48;
	vert n49, n50, n51, n52, n53, n54, n55, n56, n57, n58, n59, n60, n61, n62, n63, n64;
	vert n65, n66, n67, n68, n69, n70, n71, n72, n73, n74, n75, n76;

	n1 = vert(140.0f, 5.0f, 210.0f);
	n2 = vert(140.0f, 5.0f, 170.0f);
	n3 = vert(88.0f, 5.0f, 170.0f);
	n4 = vert(88.0f, 5.0f, 210.0f);
	n5 = vert(36.0f, 5.0f, 210.0f);

	n6 = vert(36.0f, 5.0f, 170.0f);
	n7 = vert(80.0f, 5.0f, 135.0f);
	n8 = vert(10.0f, 5.0f, 130.0f);
	n9 = vert(80.0f, 5.0f, 90.0f);
	n10 = vert(10.0f, 5.0f, 82.0f);

	n11 = vert(80.0f, 5.0f, 45.0f);
	n12 = vert(10.0f, 5.0f, 35.0f);
	n13 = vert(80.0f, 5.0f, 0.0f);
	n14 = vert(10.0f, 5.0f, -12.0f);
	n15 = vert(36.0f, 5.0f, -45.0f);

	n16 = vert(80.0f, 5.0f, -40.0f);
	n17 = vert(-24.0f, 5.0f, 0.0f);
	n18 = vert(-24.0f, 5.0f, 70.0f);
	n19 = vert(-24.0f, 5.0f, 130.0f);
	n20 = vert(-82.0f, 5.0f, 130.0f);

	n21 = vert(-82.0f, 5.0f, 70.0f);
	n22 = vert(-82.0f, 5.0f, 0.0f);
	n23 = vert(-122.0f, 5.0f, 0.0f);
	n24 = vert(-122.0f, 5.0f, 60.0f);
	n25 = vert(-122.0f, 5.0f, 130.0f);

	n26 = vert(-122.0f, 5.0f, 172.0f);
	n27 = vert(-82.0f, 5.0f, 172.0f);
	n28 = vert(-82.0f, 5.0f, 215.0f);
	n29 = vert(-122.0f, 5.0f, 215.0f);
	n30 = vert(-162.0f, 5.0f, 215.0f);

	n31 = vert(-162.0f, 5.0f, 172.0f);
	n32 = vert(-162.0f, 5.0f, 130.0f);
	n33 = vert(-170.0f, 5.0f, 100.0f);
	n34 = vert(-175.0f, 5.0f, 70.0f);
	n35 = vert(-150.0f, 5.0f, 30.0f);

	n36 = vert(-150.0f, 5.0f, -15.0f);
	n37 = vert(-122.0f, 5.0f, -30.0f);
	n38 = vert(-150.0f, 5.0f, -45.0f);
	n39 = vert(-122.0f, 5.0f, -60.0f);
	n40 = vert(-143.0f, 5.0f, -70.0f);

	n41 = vert(-130.0f, 5.0f, -90.0f);
	n42 = vert(-150.0f, 5.0f, -135.0f);
	n43 = vert(-160.0f, 5.0f, -120.0f);
	n44 = vert(-190.0f, 5.0f, -170.0f);
	n45 = vert(-175.0f, 5.0f, -175.0f);

	n46 = vert(-152.0f, 5.0f, -225.0f);
	n47 = vert(-106.0f, 5.0f, -180.0f);
	n48 = vert(-106.0f, 5.0f, -225.0f);
	n49 = vert(-60.0f, 5.0f, -225.0f);
	n50 = vert(-60.0f, 5.0f, -180.0f);

	n51 = vert(-14.0f, 5.0f, -180.0f);
	n52 = vert(-14.0f, 5.0f, -225.0f);
	n53 = vert(32.0f, 5.0f, -180.0f);
	n54 = vert(32.0f, 5.0f, -225.0f);
	n55 = vert(80.0f, 5.0f, -180.0f);

	n56 = vert(80.0f, 5.0f, -225.0f);
	n57 = vert(-220.0f, 5.0f, -180.0f);
	n58 = vert(-198.0f, 5.0f, -225.0f);
	n59 = vert(-224.0f, 5.0f, -225.0f);
	n60 = vert(-290.0f, 5.0f, -225.0f);

	n61 = vert(-260.0f, 5.0f, -180.0f);
	n62 = vert(-260.0f, 5.0f, -135.0f);
	n63 = vert(-290.0f, 5.0f, -180.0f);
	n64 = vert(-290.0f, 5.0f, -135.0f);
	n65 = vert(-260.0f, 5.0f, -90.0f);

	n66 = vert(-260.0f, 5.0f, -45.0f);
	n67 = vert(-290.0f, 5.0f, -90.0f);
	n68 = vert(-290.0f, 5.0f, -45.0f);
	n69 = vert(-260.0f, 5.0f, 0.0f);
	n70 = vert(-290.0f, 5.0f, 0.0f);

	n71 = vert(-260.0f, 5.0f, 45.0f);
	n72 = vert(-290.0f, 5.0f, 45.0f);
	n73 = vert(-290.0f, 5.0f, 90.0f);
	n74 = vert(-290.0f, 5.0f, 135.0f);
	n75 = vert(-220.0f, 5.0f, 120.0f);

	n76 = vert(-240.0f, 5.0f, 90.0f);

	MeshSegment* m1 = new MeshSegment(n1, n2, n4);
	MeshSegment* m2 = new MeshSegment(n4, n2, n3);
	MeshSegment* m3 = new MeshSegment(n4, n3, n6);
	MeshSegment* m4 = new MeshSegment(n5, n4, n6);
	MeshSegment* m5 = new MeshSegment(n3, n7, n6);

	MeshSegment* m6 = new MeshSegment(n6, n7, n8);
	MeshSegment* m7 = new MeshSegment(n8, n7, n9);
	MeshSegment* m8 = new MeshSegment(n8, n9, n10);
	MeshSegment* m9 = new MeshSegment(n10, n9, n12);
	MeshSegment* m10 = new MeshSegment(n9, n11, n12);

	MeshSegment* m11 = new MeshSegment(n12, n11, n13);
	MeshSegment* m12 = new MeshSegment(n12, n13, n14);
	MeshSegment* m13 = new MeshSegment(n14, n13, n15);
	MeshSegment* m14 = new MeshSegment(n13, n16, n15);
	MeshSegment* m15 = new MeshSegment(n12, n14, n17);

	MeshSegment* m16 = new MeshSegment(n10, n12, n17);
	MeshSegment* m17 = new MeshSegment(n18, n10, n17);
	MeshSegment* m18 = new MeshSegment(n19, n10, n18);
	MeshSegment* m19 = new MeshSegment(n19, n8, n10);
	MeshSegment* m20 = new MeshSegment(n20, n19, n21);

	MeshSegment* m21 = new MeshSegment(n19, n18, n21);
	MeshSegment* m22 = new MeshSegment(n21, n18, n17);
	MeshSegment* m23 = new MeshSegment(n21, n17, n22);
	MeshSegment* m24 = new MeshSegment(n21, n22, n23);
	MeshSegment* m25 = new MeshSegment(n24, n21, n23);

	MeshSegment* m26 = new MeshSegment(n25, n21, n24);
	MeshSegment* m27 = new MeshSegment(n25, n20, n21);
	MeshSegment* m28 = new MeshSegment(n26, n20, n25);
	MeshSegment* m29 = new MeshSegment(n26, n27, n20);
	MeshSegment* m30 = new MeshSegment(n29, n27, n26);

	MeshSegment* m31 = new MeshSegment(n29, n28, n27);
	MeshSegment* m32 = new MeshSegment(n30, n29, n31);
	MeshSegment* m33 = new MeshSegment(n31, n29, n26);
	MeshSegment* m34 = new MeshSegment(n31, n26, n32);
	MeshSegment* m35 = new MeshSegment(n32, n26, n25);

	MeshSegment* m36 = new MeshSegment(n32, n25, n33);
	MeshSegment* m37 = new MeshSegment(n33, n25, n25);
	MeshSegment* m38 = new MeshSegment(n33, n24, n34);
	MeshSegment* m39 = new MeshSegment(n34, n24, n35);
	MeshSegment* m40 = new MeshSegment(n24, n23, n35);

	MeshSegment* m41 = new MeshSegment(n35, n23, n36);
	MeshSegment* m42 = new MeshSegment(n36, n23, n37);
	MeshSegment* m43 = new MeshSegment(n36, n37, n39);
	MeshSegment* m44 = new MeshSegment(n38, n36, n39);
	MeshSegment* m45 = new MeshSegment(n38, n39, n40);

	MeshSegment* m46 = new MeshSegment(n40, n39, n41);
	MeshSegment* m47 = new MeshSegment(n40, n41, n42);
	MeshSegment* m48 = new MeshSegment(n43, n40, n42);
	MeshSegment* m49 = new MeshSegment(n44, n43, n42);
	MeshSegment* m50 = new MeshSegment(n44, n42, n45);

	MeshSegment* m51 = new MeshSegment(n44, n45, n46);
	MeshSegment* m52 = new MeshSegment(n57, n44, n46);
	MeshSegment* m53 = new MeshSegment(n46, n45, n47);
	MeshSegment* m54 = new MeshSegment(n46, n47, n48);
	MeshSegment* m55 = new MeshSegment(n48, n47, n49);

	MeshSegment* m56 = new MeshSegment(n47, n50, n49);
	MeshSegment* m57 = new MeshSegment(n49, n50, n51);
	MeshSegment* m58 = new MeshSegment(n49, n51, n52);
	MeshSegment* m59 = new MeshSegment(n52, n51, n53);
	MeshSegment* m60 = new MeshSegment(n51, n54, n53);

	MeshSegment* m61 = new MeshSegment(n53, n54, n55);
	MeshSegment* m62 = new MeshSegment(n53, n55, n56);
	MeshSegment* m63 = new MeshSegment(n58, n57, n46);
	MeshSegment* m64 = new MeshSegment(n59, n57, n58);
	MeshSegment* m65 = new MeshSegment(n59, n61, n57);

	MeshSegment* m66 = new MeshSegment(n60, n61, n59);
	MeshSegment* m67 = new MeshSegment(n60, n63, n61);
	MeshSegment* m68 = new MeshSegment(n63, n62, n61);
	MeshSegment* m69 = new MeshSegment(n64, n62, n63);
	MeshSegment* m70 = new MeshSegment(n64, n65, n62);

	MeshSegment* m71 = new MeshSegment(n64, n67, n65);
	MeshSegment* m72 = new MeshSegment(n67, n68, n65);
	MeshSegment* m73 = new MeshSegment(n68, n66, n65);
	MeshSegment* m74 = new MeshSegment(n68, n69, n66);
	MeshSegment* m75 = new MeshSegment(n68, n70, n69);

	MeshSegment* m76 = new MeshSegment(n70, n72, n69);
	MeshSegment* m77 = new MeshSegment(n72, n71, n69);
	MeshSegment* m78 = new MeshSegment(n72, n76, n71);
	MeshSegment* m79 = new MeshSegment(n72, n73, n76);
	MeshSegment* m80 = new MeshSegment(n73, n74, n76);

	MeshSegment* m81 = new MeshSegment(n74, n75, n76);
	MeshSegment* m82 = new MeshSegment(n76, n75, n33);
	MeshSegment* m83 = new MeshSegment(n76, n33, n34);

	m1->id_ = 1;
	m2->id_ = 2;
	m3->id_ = 3;
	m4->id_ = 4;
	m5->id_ = 5;
	m6->id_ = 6;
	m7->id_ = 7;
	m8->id_ = 8;
	m9->id_ = 9;
	m10->id_ = 10;
	m11->id_ = 11;
	m12->id_ = 12;
	m13->id_ = 13;
	m14->id_ = 14;
	m15->id_ = 15;
	m16->id_ = 16;
	m17->id_ = 17;
	m18->id_ = 18;
	m19->id_ = 19;
	m20->id_ = 20;
	m21->id_ = 21;
	m22->id_ = 22;
	m23->id_ = 23;
	m24->id_ = 24;
	m25->id_ = 25;
	m26->id_ = 26;
	m27->id_ = 27;
	m28->id_ = 28;
	m29->id_ = 29;
	m30->id_ = 30;
	m31->id_ = 31;
	m32->id_ = 32;
	m33->id_ = 33;
	m34->id_ = 34;
	m35->id_ = 35;
	m36->id_ = 36;
	m37->id_ = 37;
	m38->id_ = 38;
	m39->id_ = 39;
	m40->id_ = 40;
	m41->id_ = 41;
	m42->id_ = 42;
	m43->id_ = 43;
	m44->id_ = 44;
	m45->id_ = 45;
	m46->id_ = 46;
	m47->id_ = 47;
	m48->id_ = 48;
	m49->id_ = 49;
	m50->id_ = 50;
	m51->id_ = 51;
	m52->id_ = 52;
	m53->id_ = 53;
	m54->id_ = 54;
	m55->id_ = 55;
	m56->id_ = 56;
	m57->id_ = 57;
	m58->id_ = 58;
	m59->id_ = 59;
	m60->id_ = 60;
	m61->id_ = 61;
	m62->id_ = 62;
	m63->id_ = 63;
	m64->id_ = 64;
	m65->id_ = 65;
	m66->id_ = 66;
	m67->id_ = 67;
	m68->id_ = 68;
	m69->id_ = 69;
	m70->id_ = 70;
	m71->id_ = 71;
	m72->id_ = 72;
	m73->id_ = 73;
	m74->id_ = 74;
	m75->id_ = 75;
	m76->id_ = 76;
	m77->id_ = 77;
	m78->id_ = 78;
	m79->id_ = 79;
	m80->id_ = 80;
	m81->id_ = 81;
	m82->id_ = 82;
	m83->id_ = 83;

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
	mesh_.push_back(m45);
	mesh_.push_back(m46);
	mesh_.push_back(m47);
	mesh_.push_back(m48);
	mesh_.push_back(m49);
	mesh_.push_back(m50);
	mesh_.push_back(m51);
	mesh_.push_back(m52);
	mesh_.push_back(m53);
	mesh_.push_back(m54);
	mesh_.push_back(m55);
	mesh_.push_back(m56);
	mesh_.push_back(m57);
	mesh_.push_back(m58);
	mesh_.push_back(m59);
	mesh_.push_back(m60);
	mesh_.push_back(m61);
	mesh_.push_back(m62);
	mesh_.push_back(m63);
	mesh_.push_back(m64);
	mesh_.push_back(m65);
	mesh_.push_back(m66);
	mesh_.push_back(m67);
	mesh_.push_back(m68);
	mesh_.push_back(m69);
	mesh_.push_back(m70);
	mesh_.push_back(m71);
	mesh_.push_back(m72);
	mesh_.push_back(m73);
	mesh_.push_back(m74);
	mesh_.push_back(m75);
	mesh_.push_back(m76);
	mesh_.push_back(m77);
	mesh_.push_back(m78);
	mesh_.push_back(m79);
	mesh_.push_back(m80);
	mesh_.push_back(m81);
	mesh_.push_back(m82);
	mesh_.push_back(m83);

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