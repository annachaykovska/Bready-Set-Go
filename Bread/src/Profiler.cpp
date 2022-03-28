#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "Profiler.h"
#include "Window.h"
#include "Scene/Scene.h"
#include "Transform.h"
#include "Scene/Entity.h"
#include "Inventory.h"
#include "SystemManager.h"
#include "Rendering/RenderingSystem.h"
#include "Rendering/UISystem.h"

extern Scene g_scene;
extern SystemManager g_systems;

Profiler::Profiler(Window& window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window.getWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 330");

	frameCounter = 0;
	t0 = glfwGetTime();
	msPerFrame = 0;
	framesPerSecond = 0;

	depthMap = 0;
}

void Profiler::newFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Profiler::cleanup()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Profiler::begin()
{
	bool closable = true;
	ImGui::Begin("Profiler", &closable);
	ImGui::SetWindowFontScale(2.0f);
}

void Profiler::end()
{
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Profiler::fps()
{
	frameCounter++;
	double t1 = glfwGetTime();
	if (t1 - t0 >= 1.0)
	{
		msPerFrame = 1000.0 / (double)frameCounter;
		framesPerSecond = frameCounter / (t1 - t0);
		frameCounter = 0;
		t0 += 1.0;
	}

	ImGui::Text("msPerFrame = %lf", msPerFrame);
	ImGui::Text("framesPerSecond = %lf", framesPerSecond);
}

void Profiler::player1Inventory()
{
	Entity* player1 = g_scene.getEntity("player1");
	Inventory* p1Inv = (Inventory*)player1->getComponent("inventory");

	ImGui::Text("Current Recipe:");
	ImGui::Text("Tomato: %d/1", p1Inv->tomato);
	ImGui::Text("Cheese: %d/1", p1Inv->cheese);
	ImGui::Text("Sausage: %d/1", p1Inv->sausage);
	ImGui::Text("Dough: %d/1", p1Inv->dough);

	if (p1Inv->tomato == 1 && p1Inv->cheese == 1 && p1Inv->sausage == 1 && p1Inv->dough == 1)
		ImGui::Text("Recipe complete!");
	else
		ImGui::Text("Complete the recipe!");
}

void Profiler::player1Transform()
{
	Entity* player1 = g_scene.getEntity("player1");
	Transform* player1Transform = player1->getTransform();

	if (ImGui::CollapsingHeader("Player 1"))
	{
		ImGui::InputFloat("p1.pos.x", &(player1Transform->position.x), 1.f, 10.f, "%.3f");
		ImGui::InputFloat("p1.pos.y", &(player1Transform->position.y), 1.f, 10.f, "%.3f");
		ImGui::InputFloat("p1.pos.z", &(player1Transform->position.z), 1.f, 10.f, "%.3f");

		//ImGui::SliderFloat("xrot", &(player1Transform->rotation.x), -180.0f, 180.0f, "%.3f", 1.0f);
		//ImGui::SliderFloat("yrot", &(player1Transform->rotation.y), -180.0f, 180.0f, "%.3f", 1.0f);
		//ImGui::SliderFloat("zrot", &(player1Transform->rotation.z), -180.0f, 180.0f, "%.3f", 1.0f);
	}
}

void Profiler::player2Transform()
{
	Entity* player2 = g_scene.getEntity("player2");
	Transform* player2Transform = player2->getTransform();

	if (ImGui::CollapsingHeader("Player 2"))
	{
		ImGui::InputFloat("p2.pos.x", &(player2Transform->position.x), 1.f, 10.f, "%.3f");
		ImGui::InputFloat("p2.pos.y", &(player2Transform->position.y), 1.f, 10.f, "%.3f");
		ImGui::InputFloat("p2.pos.z", &(player2Transform->position.z), 1.f, 10.f, "%.3f");

		//ImGui::SliderFloat("xrot2", &(player2Transform->rotation.x), -180.0f, 180.0f, "%.3f", 1.0f);
		//ImGui::SliderFloat("yrot2", &(player2Transform->rotation.y), -180.0f, 180.0f, "%.3f", 1.0f);
		//ImGui::SliderFloat("zrot2", &(player2Transform->rotation.z), -180.0f, 180.0f, "%.3f", 1.0f);
	}
}

void Profiler::player3Transform()
{
	Entity* player3 = g_scene.getEntity("player3");
	Transform* player3Transform = player3->getTransform();

	if (ImGui::CollapsingHeader("Player 3"))
	{
		ImGui::InputFloat("p3.pos.x", &(player3Transform->position.x), 1.f, 10.f, "%.3f");
		ImGui::InputFloat("p3.pos.y", &(player3Transform->position.y), 1.f, 10.f, "%.3f");
		ImGui::InputFloat("p3.pos.z", &(player3Transform->position.z), 1.f, 10.f, "%.3f");

		//ImGui::SliderFloat("xrot2", &(player2Transform->rotation.x), -180.0f, 180.0f, "%.3f", 1.0f);
		//ImGui::SliderFloat("yrot2", &(player2Transform->rotation.y), -180.0f, 180.0f, "%.3f", 1.0f);
		//ImGui::SliderFloat("zrot2", &(player2Transform->rotation.z), -180.0f, 180.0f, "%.3f", 1.0f);
	}
}

void Profiler::player4Transform()
{
	Entity* player4 = g_scene.getEntity("player4");
	Transform* player4Transform = player4->getTransform();

	if (ImGui::CollapsingHeader("Player 4"))
	{
		ImGui::InputFloat("p4.pos.x", &(player4Transform->position.x), 1.f, 10.f, "%.3f");
		ImGui::InputFloat("p4.pos.y", &(player4Transform->position.y), 1.f, 10.f, "%.3f");
		ImGui::InputFloat("p4.pos.z", &(player4Transform->position.z), 1.f, 10.f, "%.3f");

		//ImGui::SliderFloat("xrot2", &(player2Transform->rotation.x), -180.0f, 180.0f, "%.3f", 1.0f);
		//ImGui::SliderFloat("yrot2", &(player2Transform->rotation.y), -180.0f, 180.0f, "%.3f", 1.0f);
		//ImGui::SliderFloat("zrot2", &(player2Transform->rotation.z), -180.0f, 180.0f, "%.3f", 1.0f);
	}
}

void Profiler::cameraTransform()
{
	Camera* camera = &(g_scene.camera);

	float xBuf[50];
	float step = 1.f;
	float stepFast = 10.f;

	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::InputFloat("cameraXPos", &(camera->position.x), step, stepFast, "%.3f");
		ImGui::InputFloat("cameraYPos", &(camera->position.y), step, stepFast, "%.3f");
		ImGui::InputFloat("cameraZPos", &(camera->position.z), step, stepFast, "%.3f");

		ImGui::InputFloat("cameraYaw", &(camera->yaw), step, stepFast, "%.3f");
		ImGui::InputFloat("cameraPitch", &(camera->pitch), step, stepFast, "%.3f");
	}
}

void Profiler::testTransform()
{
	Entity* test = g_scene.getEntity("test");
	Transform* testTransform = test->getTransform();

	if (ImGui::CollapsingHeader("Test"))
	{
		ImGui::InputFloat("test.pos.x", &(testTransform->position.x), 1.f, 10.f, "%.3f");
		ImGui::InputFloat("test.pos.y", &(testTransform->position.y), 1.f, 10.f, "%.3f");
		ImGui::InputFloat("test.pos.z", &(testTransform->position.z), 1.f, 10.f, "%.3f");

		ImGui::SliderFloat("test.rot.x", &(testTransform->rotation.x), -180.0f, 180.0f, "%.3f", 1.0f);
		ImGui::SliderFloat("test.rot.y", &(testTransform->rotation.y), -180.0f, 180.0f, "%.3f", 1.0f);
		ImGui::SliderFloat("test.rot.z", &(testTransform->rotation.z), -180.0f, 180.0f, "%.3f", 1.0f);
	}
}

void Profiler::shadows()
{
	Orthogonal& ort = g_systems.render->ort;
	glm::vec3& lightDir = g_systems.render->lightDir;
	glm::vec3& lightPos = g_systems.render->lightPos;
	
	if (ImGui::CollapsingHeader("Shadows"))
	{
		ImGui::InputInt("DepthMap Render", &depthMap);

		if (depthMap == 0)
			g_systems.renderDebug = false;
		if (depthMap == 1)
		{
			g_systems.renderDebug = true;
			g_systems.render->shadowDebugMode = 1;
		}
		if (depthMap == 2)
		{
			g_systems.renderDebug = true;
			g_systems.render->shadowDebugMode = 2;
		}

		ImGui::InputFloat("orthoProj.left", &(ort.left), 1.f, 10.f, "%.3f");
		ImGui::InputFloat("orthoProj.right", &(ort.right), 1.f, 10.f, "%.3f");
		ImGui::InputFloat("orthoProj.bottom", &(ort.bottom), 1.f, 10.f, "%.3f");
		ImGui::InputFloat("orthoProj.top", &(ort.top), 1.f, 10.f, "%.3f");
		ImGui::InputFloat("orthoProj.near", &(ort.nearPlane), 1.f, 10.f, "%.3f");
		ImGui::InputFloat("orthoProj.far", &(ort.farPlane), 1.f, 10.f, "%.3f");

		ImGui::InputFloat("lightPos.x", &(lightPos.x), 1.f, 10.f, "%.3f");
		ImGui::InputFloat("lightPos.y", &(lightPos.y), 1.f, 10.f, "%.3f");
		ImGui::InputFloat("lightPos.z", &(lightPos.z), 1.f, 10.f, "%.3f");
		
		//float& maxBias = g_systems.render->maxBias;

		//ImGui::InputFloat("maxBias", &(maxBias), 0.0001f, 0.01f,  "%.5f");
		//ImGui::InputFloat("maxBias", &(g_systems.render->maxBias), 0.0001f, 0.01f, "%.5f");
		//ImGui::InputFloat("minBias", &(g_systems.render->minBias), 0.0001f, 0.01f, "%.5f");
		//ImGui::InputFloat("maxRoughBias", &(g_systems.render->maxRoughBias), 0.0001f, 0.01f, "%.5f");
		//ImGui::InputFloat("minRoughBias", &(g_systems.render->minRoughBias), 0.0001f, 0.01f, "%.5f");
	}
}

void Profiler::meshScale()
{
	Transform* kitchenTrans = g_scene.getEntity("countertop")->getTransform();

	if (ImGui::CollapsingHeader("Mesh Scaler"))
	{
		ImGui::InputFloat("kitchen.pos.x", &(kitchenTrans->position.x), 1.f, 10.f, "%.3f");
		ImGui::InputFloat("kitchen.pos.y", &(kitchenTrans->position.y), 1.f, 10.f, "%.3f");
		ImGui::InputFloat("kitchen.pos.z", &(kitchenTrans->position.z), 1.f, 10.f, "%.3f");
		ImGui::InputFloat("kitchen.scale.x", &(kitchenTrans->scale.x), 1.f, 10.f, "%.3f");
		ImGui::InputFloat("kitchen.scale.y", &(kitchenTrans->scale.y), 1.f, 10.f, "%.3f");
		ImGui::InputFloat("kitchen.scale.z", &(kitchenTrans->scale.z), 1.f, 10.f, "%.3f");
	}
}

void Profiler::physicsValues() {
	ImGui::InputFloat("x", &(g_systems.ui->map_x), 0.01f, .1f, "%.3f");		// WheelMass
	ImGui::InputFloat("sx", &(g_systems.ui->map_sx), 0.01f, .1f, "%.3f");		// WheelMass
	PhysicsSystem* physSys = g_systems.physics;
	//physSys->mVehiclePlayer1
	//std::cout << physSys->chassis_mass << std::endl;
	if (ImGui::CollapsingHeader("Physics Values")) {
		if (ImGui::Button("Reload", ImVec2(100, 30))) {	// Restarts game
			g_systems.physics->updateCar();
		}
		//if (ImGui::Button("Reload but Harder", ImVec2(300, 30))) {
		//	g_systems.physics->initialize();
		//}
		if (ImGui::CollapsingHeader("Wheels")) {
			ImGui::InputFloat("Wheel Mass", &(physSys->wheel_mass), 10.f, 100.f, "%.3f");		// WheelMass
			ImGui::InputFloat("Wheel MOI", &(physSys->wheel_moi), 10.f, 100.f, "%.3f");			// WheelMOI
			ImGui::InputFloat("Damping Rate", &(physSys->wheel_damping_rate), 10.f, 100.f, "%.3f");	// Damping Rate
			ImGui::InputFloat("Max hand brake torque", &(physSys->max_hand_brake_torque), 10.f, 100.f, "%.3f");
			ImGui::InputFloat("Max Steer", &(physSys->max_steer), 3.14 / 60.f, 3.14f / 6.f, "%.3f");
			ImGui::InputFloat("Max Brake Torque", &(physSys->max_brake_torque), 10.f, 100.f, "%.3f");	
		}
		if (ImGui::CollapsingHeader("Suspension")) {
			ImGui::InputFloat("Spring Strength", &(physSys->spring_strength), 100.f, 1000.f, "%.3f");
			ImGui::InputFloat("Max Compression", &(physSys->max_compression), .01f, .1f, "%.3f");
			ImGui::InputFloat("Max Droop", &(physSys->max_droop), 0.01f, .1f, "%.3f");
			//ImGui::InputFloat("Spring Min", &(physSys->spring_strength), 10.f, 100.f, "%.3f");


			ImGui::InputFloat("Camber at rest", &(physSys->camber_at_rest), 3.14 / 60.f, 3.14f / 6.f, "%.3f");
		}

		if (ImGui::CollapsingHeader("Engine")) {
			// engine
			ImGui::InputFloat("Peak Torque", &(physSys->peak_torque), 10.f, 100.f, "%.3f");	// enging peak torque
			ImGui::InputFloat("Max Omega", &(physSys->max_omega), 10.f, 100.f, "%.3f");		// engine max Omega
		}
		if (ImGui::CollapsingHeader("Chassis")) {
			ImGui::InputFloat("Chassis MOI y", &(physSys->chassis_moi_y), 0.01f, 0.1f, "%.3f");	// Chassis MOI y
		}
	}
}

void Profiler::cameraValues() {
	auto camera = g_scene.camera;
	if (ImGui::CollapsingHeader("Camera Values")) {
		//FIXME: THESE DOESNT CHANGE THE VALUES
		//ImGui::InputFloat("a", &(camera.a), 10.f, 100.f, "%.3f");
		//ImGui::InputFloat("b", &(camera.b), 10.f, 100.f, "%.3f");
		//ImGui::InputFloat("c", &(camera.c), 10.f, 100.f, "%.3f");
		//ImGui::InputFloat("d", &(camera.d), 10.f, 100.f, "%.3f");
	}
}

void Profiler::update()
{
	begin();
	fps();
	//player1Inventory();
	//cameraValues(); //doesn't really work rn I DONT KNOW WHY
	physicsValues();
	player1Transform();
	player2Transform();
	player3Transform();
	player4Transform();
	cameraTransform();	
	testTransform();
	shadows();
	meshScale();
	end();
}