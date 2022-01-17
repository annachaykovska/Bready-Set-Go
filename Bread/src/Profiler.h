#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

class Profiler {
public:

	Profiler(Window &window) 
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

	void newFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void cleanup()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void begin()
	{
		bool closable = true;
		ImGui::Begin("Profiler", &closable);
		ImGui::SetWindowFontScale(2.0f);
	}

	void end()
	{
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void fps()
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

	void run()
	{
		newFrame();
		begin();
		fps();
		end();
	}

private:

	unsigned int frameCounter;
	double t0;
	double msPerFrame;
	double framesPerSecond;
};