#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "Profiler.h"
#include "Window.h"
#include "Scene/Scene.h"
#include "Transform.h"
#include "Scene/Entity.h"

extern Scene g_scene;

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

void Profiler::transform()
{
	Entity* player1 = g_scene.getEntity("player1");
	Transform* player1Transform = player1->getTransform();

	std::string x = std::to_string(player1Transform->position.x);
	std::string y = std::to_string(player1Transform->position.y);
	std::string z = std::to_string(player1Transform->position.z);

	std::string text = "\nPlayer1 position: ";

	//ImGui::Text(text.c_str());
	ImGui::SliderFloat("xpos", &(player1Transform->position.x), -100.0f, 100.0f, x.c_str(), 1.0f);
	ImGui::SliderFloat("ypos", &(player1Transform->position.y), -100.0f, 100.0f, y.c_str(), 1.0f);
	ImGui::SliderFloat("zpos", &(player1Transform->position.z), -100.0f, 100.0f, z.c_str(), 1.0f);

	x = std::to_string(player1Transform->rotation.x);
	y = std::to_string(player1Transform->rotation.y);
	z = std::to_string(player1Transform->rotation.z);

	text = "\nPlayer1 rotation: ";

	ImGui::Text(text.c_str());
	ImGui::SliderFloat("xrot", &(player1Transform->rotation.x), -180.0f, 180.0f, x.c_str(), 1.0f);
	ImGui::SliderFloat("yrot", &(player1Transform->rotation.y), -180.0f, 180.0f, y.c_str(), 1.0f);
	ImGui::SliderFloat("zrot", &(player1Transform->rotation.z), -180.0f, 180.0f, z.c_str(), 1.0f);
}

void Profiler::update()
{
	begin();
	fps();
	transform();
	end();
}