#include "Game.h"
#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include "GameStateManager.h"
#include "SplashState.h"
#include "Procedural.h"
#include "TestState.h"
#include "MazeState.h"
#include <Windows.h>
#include <conio.h>
#include <stdio.h>
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb-master\stb_image.h>
#include <time.h>
Game::Game() {
	srand(time(NULL));
	currentFrame = 0;
	deltaTime = 0;
	lastFrame = 0;
	
	if (glfwInit() == false) {
		printf("Failed to initialise GLFW");
		return;
	}
	m_gameWindow = glfwCreateWindow(1240, 768, "Assignment Two - Shane Coates", nullptr, nullptr);
	if (m_gameWindow == nullptr) {
		printf("Failed to create Game Window");
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(m_gameWindow);
	if (ogl_LoadFunctions() == ogl_LOAD_FAILED) {
		printf("Failed to load OpenGL Functions");
		glfwDestroyWindow(m_gameWindow);
		glfwTerminate();
		return;
	}
	
	ImGui_ImplGlfwGL3_Init(m_gameWindow, true);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	m_gameStateManager = new GameStateManager();
	m_gameStateManager->RegisterState("Maze", new MazeState(m_gameWindow, m_gameStateManager));
	m_gameStateManager->RegisterState("Test", new TestState(m_gameWindow, m_gameStateManager));
	m_gameStateManager->Push("Maze");
}
Game::~Game() {
	delete m_gameStateManager;
	glfwDestroyWindow(m_gameWindow);
	glfwTerminate();
	ImGui_ImplGlfwGL3_Shutdown();
}

void Game::Run() {
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	while (glfwWindowShouldClose(m_gameWindow) == false) {
		double dt = GetDeltaTime();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplGlfwGL3_NewFrame();
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Separator();

		m_gameStateManager->Update(dt);
		m_gameStateManager->Draw();
		
		ImGui::Render();
		
		glfwSwapBuffers(m_gameWindow);
		glfwPollEvents();
	}
}

double Game::GetDeltaTime(){
	currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	return deltaTime;
}