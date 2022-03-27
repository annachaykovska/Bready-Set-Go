#include "NavMesh.h"

NavMesh::NavMesh()
{
	vert n1, n2, n3, n4, n5, n6, n7, n8, n9, n10;
	vert n11, n12, n13, n14, n15, n16, n17, n18, n19, n20;
	vert n21, n22, n23, n24, n25, n26, n27, n28, n29, n30;
	vert n31, n32, n33, n34, n35, n36, n37, n38, n39, n40;
	vert n41, n42, n43, n44, n45, n46, n47, n48, n49, n50;
	vert n51, n52, n53, n54, n55, n56, n57, n58, n59, n60;
	vert n61, n62, n63, n64, n65, n66, n67, n68, n69, n70;
	vert n71, n72, n73, n74, n75, n76, n77, n78, n79, n80;
	vert n81, n82, n83, n84, n85, n86, n87, n88, n89, n90;
	vert n91, n92, n93, n94, n95, n96, n97, n98, n99, n100;
	vert n101, n102, n103, n104, n105, n106, n107, n108, n109, n110;
	vert n111, n112, n113, n114, n115, n116, n117, n118, n119, n120;
	vert n121, n122, n123, n124, n125, n126, n127, n128, n129, n130;
	vert n131, n132;

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

	// COUNTER LEVEL SHELVES
	n45 = vert(-10, 6, 162);
	n46 = vert(-90, 6, 162);
	n47 = vert(-90, 6, 203);
	n48 = vert(-10, 6, 203);
	n49 = vert(72, 6, 203);
	n50 = vert(82, 6, 185);
	n51 = vert(92, 6, 203);
	n52 = vert(205, 6, 203);
	n53 = vert(173, 6, 165);
	n54 = vert(205, 6, 165);
	n55 = vert(195, 6, 131);
	n56 = vert(199, 6, 101);
	n57 = vert(191, 6, 101);
	n58 = vert(195, 6, 70);
	n59 = vert(225, 6, 41);
	n60 = vert(166, 6, 41);
	n61 = vert(221, 6, -51);
	n62 = vert(195, 6, -66);
	n63 = vert(237, 6, -60);
	n64 = vert(221, 6, -150);
	n65 = vert(170, 6, -150);
	n66 = vert(259, 6, -63);

	// SHELVES
	n67 = vert(-96, 58, 184);
	n68 = vert(-43, 58, 179);
	n69 = vert(-43, 58, 205);
	n70 = vert(20, 81, 205);
	n71 = vert(20, 81, 179);
	n72 = vert(93, 108, 179);
	n73 = vert(93, 108, 205);
	n74 = vert(123, 108, 206);
	n75 = vert(93, 108, 143);
	n76 = vert(126, 108, 143);
	n77 = vert(156, 108, 168);
	n78 = vert(206, 108, 205);
	n79 = vert(206, 108, 143);
	n80 = vert(164, 108, 143);
	n81 = vert(172, 108, 91);
	n82 = vert(206, 108, 91);
	n83 = vert(267, 108, 91);
	n84 = vert(219, 108, -35);
	n85 = vert(172, 108, -159);
	n86 = vert(267, 108, -159);
	n87 = vert(-40, 108, 143);
	n88 = vert(-40, 108, 174);
	n89 = vert(194, 58, 143);
	n90 = vert(194, 58, 179);

	// UNDER
	n91 = vert(-61, -27, -149);
	n92 = vert(-61, -27, -129);
	n93 = vert(-99, -30, -146);
	n94 = vert(-99, -30, -134);
	n95 = vert(-143, -39, -146);

	n96 = vert(-143, -39, -134);
	n97 = vert(-192, -63, -146);
	n98 = vert(-192, -63, -132);
	n99 = vert(-264, -63, -153);
	n100 = vert(-226, -63, -112);

	n101 = vert(-264, -63, -106);
	n102 = vert(-226, -63, -74);
	n103 = vert(-264, -63, -32);
	n104 = vert(-114, -63, -32);
	n105 = vert(-114, -63, -70);

	n106 = vert(-61, -63, -79);
	n107 = vert(-76, -63, -50);
	n108 = vert(-68, -63, -15);
	n109 = vert(-88, -63, -15);
	//n110 = vert();

	n111 = vert(-104, -63, 26);
	n112 = vert(-58, -63, 26);
	n113 = vert(-77, -63, 26);
	n114 = vert(-73, -63, 67);
	n115 = vert(-80, -63, 67);

	n116 = vert(-83, -63, 150);
	n117 = vert(-76, -63, 151);
	n118 = vert(-79, -63, 174);
	n119 = vert(-98, -63, 203);
	n120 = vert(-49, -63, 208);

	n121 = vert(220, -63, 200);
	n122 = vert(222, -63, 158);
	n123 = vert(166, -63, 157);
	n124 = vert(193, -63, 135);
	n125 = vert(200, -63, 100);

	n126 = vert(187, -63, 100);
	n127 = vert(162, -64, 58);
	n128 = vert(196, -64, 57);
	n129 = vert(233, -64, 50);
	n130 = vert(260, -64, 49);

	n131 = vert(245, -64, -121);
	n132 = vert(162, -64, -121);

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

	// COUNTER LEVEL SHELVES
	MeshSegment* m51 = new MeshSegment(n50, n43, n44);
	MeshSegment* m52 = new MeshSegment(n49, n50, n44);
	MeshSegment* m53 = new MeshSegment(n48, n49, n44);
	MeshSegment* m54 = new MeshSegment(n48, n44, n45);
	MeshSegment* m55 = new MeshSegment(n48, n45, n46);

	MeshSegment* m56 = new MeshSegment(n48, n46, n47);
	MeshSegment* m57 = new MeshSegment(n49, n51, n50);
	MeshSegment* m58 = new MeshSegment(n51, n43, n50);
	MeshSegment* m59 = new MeshSegment(n51, n53, n43);
	MeshSegment* m60 = new MeshSegment(n51, n52, n53);

	MeshSegment* m61 = new MeshSegment(n52, n54, n53);
	MeshSegment* m62 = new MeshSegment(n53, n54, n55);
	MeshSegment* m63 = new MeshSegment(n53, n55, n57);
	MeshSegment* m64 = new MeshSegment(n54, n56, n55);
	MeshSegment* m65 = new MeshSegment(n55, n56, n57);

	MeshSegment* m66 = new MeshSegment(n57, n56, n58);
	MeshSegment* m67 = new MeshSegment(n57, n58, n60);
	MeshSegment* m68 = new MeshSegment(n56, n59, n58);
	MeshSegment* m69 = new MeshSegment(n58, n59, n60);
	MeshSegment* m70 = new MeshSegment(n60, n59, n61);
	
	MeshSegment* m71 = new MeshSegment(n60, n61, n40);
	MeshSegment* m72 = new MeshSegment(n40, n61, n62);
	MeshSegment* m73 = new MeshSegment(n40, n62, n39);
	MeshSegment* m74 = new MeshSegment(n61, n63, n62);
	MeshSegment* m75 = new MeshSegment(n62, n63, n39);

	MeshSegment* m76 = new MeshSegment(n39, n63, n65);
	MeshSegment* m77 = new MeshSegment(n63, n64, n65);
	MeshSegment* m78 = new MeshSegment(n66, n64, n63);

	// SHELVES
	MeshSegment* m79 = new MeshSegment(n68, n42, n41);
	MeshSegment* m80 = new MeshSegment(n67, n68, n41);

	MeshSegment* m81 = new MeshSegment(n67, n69, n68);
	MeshSegment* m82 = new MeshSegment(n69, n71, n68);
	MeshSegment* m83 = new MeshSegment(n69, n70, n71);
	MeshSegment* m84 = new MeshSegment(n70, n73, n71);
	MeshSegment* m85 = new MeshSegment(n73, n72, n71);

	//MeshSegment* m86 = new MeshSegment();
	MeshSegment* m87 = new MeshSegment(n73, n74, n72);
	MeshSegment* m88 = new MeshSegment(n74, n76, n72);
	MeshSegment* m89 = new MeshSegment(n72, n76, n75);
	MeshSegment* m90 = new MeshSegment(n74, n77, n76);

	MeshSegment* m91 = new MeshSegment(n74, n78, n77);
	MeshSegment* m92 = new MeshSegment(n78, n79, n77);
	MeshSegment* m93 = new MeshSegment(n77, n80, n76);
	MeshSegment* m94 = new MeshSegment(n77, n79, n80);
	MeshSegment* m95 = new MeshSegment(n79, n81, n80);

	MeshSegment* m96 = new MeshSegment(n79, n82, n81);
	MeshSegment* m97 = new MeshSegment(n81, n82, n84);
	MeshSegment* m98 = new MeshSegment(n82, n83, n84);
	MeshSegment* m99 = new MeshSegment(n83, n86, n84);
	MeshSegment* m100 = new MeshSegment(n81, n84, n85);

	MeshSegment* m101 = new MeshSegment(n84, n86, n85);
	MeshSegment* m102 = new MeshSegment(n72, n75, n87);
	MeshSegment* m103 = new MeshSegment(n88, n72, n87);
	MeshSegment* m104 = new MeshSegment(n68, n89, n42);
	MeshSegment* m105 = new MeshSegment(n68, n90, n89);

	// UNDER
	MeshSegment* m106 = new MeshSegment(n8, n7, n91);
	MeshSegment* m107 = new MeshSegment(n92, n8, n91);
	MeshSegment* m108 = new MeshSegment(n94, n92, n91);
	MeshSegment* m109 = new MeshSegment(n94, n91, n93);
	MeshSegment* m110 = new MeshSegment(n95, n94, n93);

	MeshSegment* m111 = new MeshSegment(n96, n94, n95);
	MeshSegment* m112 = new MeshSegment(n98, n96, n95);
	MeshSegment* m113 = new MeshSegment(n98, n95, n97);
	MeshSegment* m114 = new MeshSegment(n98, n97, n99);
	MeshSegment* m115 = new MeshSegment(n100, n98, n99);

	MeshSegment* m116 = new MeshSegment(n101, n100, n99);
	MeshSegment* m117 = new MeshSegment(n102, n100, n101);
	MeshSegment* m118 = new MeshSegment(n103, n102, n101);
	MeshSegment* m119 = new MeshSegment(n103, n104, n102);
	MeshSegment* m120 = new MeshSegment(n104, n105, n102);

	MeshSegment* m121 = new MeshSegment(n104, n107, n105);
	MeshSegment* m122 = new MeshSegment(n107, n106, n105);
	MeshSegment* m123 = new MeshSegment(n108, n106, n107);
	MeshSegment* m124 = new MeshSegment(n104, n109, n107);
	MeshSegment* m125 = new MeshSegment(n109, n108, n107);

	MeshSegment* m126 = new MeshSegment(n113, n108, n109);
	MeshSegment* m127 = new MeshSegment(n111, n113, n109);
	MeshSegment* m128 = new MeshSegment(n113, n112, n108);
	MeshSegment* m129 = new MeshSegment(n115, n113, n111);
	MeshSegment* m130 = new MeshSegment(n115, n114, n113);

	MeshSegment* m131 = new MeshSegment(n114, n112, n113);
	MeshSegment* m132 = new MeshSegment(n117, n114, n115);
	MeshSegment* m133 = new MeshSegment(n116, n117, n115);
	MeshSegment* m134 = new MeshSegment(n118, n117, n116);
	MeshSegment* m135 = new MeshSegment(n119, n118, n116);

	MeshSegment* m136 = new MeshSegment(n120, n117, n118);
	MeshSegment* m137 = new MeshSegment(n119, n120, n118);
	MeshSegment* m138 = new MeshSegment(n120, n123, n117);
	MeshSegment* m139 = new MeshSegment(n120, n121, n123);
	MeshSegment* m140 = new MeshSegment(n121, n122, n123);

	MeshSegment* m141 = new MeshSegment(n123, n122, n124);
	MeshSegment* m142 = new MeshSegment(n123, n124, n126);
	MeshSegment* m143 = new MeshSegment(n122, n125, n124);
	MeshSegment* m144 = new MeshSegment(n124, n125, n126);
	MeshSegment* m145 = new MeshSegment(n126, n125, n128);

	MeshSegment* m146 = new MeshSegment(n126, n128, n127);
	MeshSegment* m147 = new MeshSegment(n125, n129, n128);
	MeshSegment* m148 = new MeshSegment(n125, n130, n129);
	MeshSegment* m149 = new MeshSegment(n127, n128, n132);
	MeshSegment* m150 = new MeshSegment(n128, n131, n132);

	MeshSegment* m151 = new MeshSegment(n128, n129, n131);
	MeshSegment* m152 = new MeshSegment(n129, n130, n63);
	MeshSegment* m153 = new MeshSegment(n130, n66, n63);

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

	// COUNTER LEVEL SHELVES
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

	// SHELVES
	mesh_.push_back(m79);
	mesh_.push_back(m80);

	mesh_.push_back(m81);
	mesh_.push_back(m82);
	mesh_.push_back(m83);
	mesh_.push_back(m84);
	mesh_.push_back(m85);

	//mesh_.push_back(m86);
	mesh_.push_back(m87);
	mesh_.push_back(m88);
	mesh_.push_back(m89);
	mesh_.push_back(m90);

	mesh_.push_back(m91);
	mesh_.push_back(m92);
	mesh_.push_back(m93);
	mesh_.push_back(m94);
	mesh_.push_back(m95);

	mesh_.push_back(m96);
	mesh_.push_back(m97);
	mesh_.push_back(m98);
	mesh_.push_back(m99);
	mesh_.push_back(m100);

	mesh_.push_back(m101);
	mesh_.push_back(m102);
	mesh_.push_back(m103);
	mesh_.push_back(m104);
	mesh_.push_back(m105);

	// UNDER
	mesh_.push_back(m106);
	mesh_.push_back(m107);
	mesh_.push_back(m108);
	mesh_.push_back(m109);
	mesh_.push_back(m110);

	mesh_.push_back(m111);
	mesh_.push_back(m112);
	mesh_.push_back(m113);
	mesh_.push_back(m114);
	mesh_.push_back(m115);

	mesh_.push_back(m116);
	mesh_.push_back(m117);
	mesh_.push_back(m118);
	mesh_.push_back(m119);
	mesh_.push_back(m120);

	mesh_.push_back(m121);
	mesh_.push_back(m122);
	mesh_.push_back(m123);
	mesh_.push_back(m124);
	mesh_.push_back(m125);

	mesh_.push_back(m126);
	mesh_.push_back(m127);
	mesh_.push_back(m128);
	mesh_.push_back(m129);
	mesh_.push_back(m130);

	mesh_.push_back(m131);
	mesh_.push_back(m132);
	mesh_.push_back(m133);
	mesh_.push_back(m134);
	mesh_.push_back(m135);

	mesh_.push_back(m136);
	mesh_.push_back(m137);
	mesh_.push_back(m138);
	mesh_.push_back(m139);
	mesh_.push_back(m140);

	mesh_.push_back(m141);
	mesh_.push_back(m142);
	mesh_.push_back(m143);
	mesh_.push_back(m144);
	mesh_.push_back(m145);

	mesh_.push_back(m146);
	mesh_.push_back(m147);
	mesh_.push_back(m148);
	mesh_.push_back(m149);
	mesh_.push_back(m150);

	mesh_.push_back(m151);
	mesh_.push_back(m152);
	mesh_.push_back(m153);

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

int NavMesh::currentMeshSegment(vert position)
{
	float min = 999999;
	int minInd = -1;

	for (int i = 0; i < mesh_.size(); i++)
	{
		float summedAngle = 0;

		glm::vec3 vectorA = mesh_.at(i)->v0_ - position;
		glm::vec3 vectorB = mesh_.at(i)->v1_ - position;
		glm::vec3 vectorC = mesh_.at(i)->v2_ - position;

		summedAngle += acos(dot(vectorA, vectorB) / (length(vectorA) * length(vectorB)));
		summedAngle += acos(dot(vectorA, vectorC) / (length(vectorA) * length(vectorC)));
		summedAngle += acos(dot(vectorB, vectorC) / (length(vectorB) * length(vectorC)));

		if (abs(summedAngle - (2 * 3.1415926)) < min)
		{
			min = abs(summedAngle - (2 * 3.1415926));
			minInd = mesh_.at(i)->id_;
		}
	}
	std::cout << minInd << " " << min << std::endl;
	return minInd;
}