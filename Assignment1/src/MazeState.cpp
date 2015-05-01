#include "MazeState.h"
#include "FlyCamera.h"
#include "imgui.h"
#include "aieutilities\Gizmos.h"
#include "Maze.h"
void MazeState::Init(GLFWwindow* _window, GameStateManager* _gameStateManager) {
	m_window = _window;
	m_gameStateManager = _gameStateManager;
	m_camera = new FlyCamera(10.0f);
	m_camera->SetInputWindow(m_window);
	m_camera->SetPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 1000.0f);
	m_camera->SetLookAt(glm::vec3(80, 50, 80), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	m_maze = new Maze();

	Gizmos::create();
}

MazeState::~MazeState() {
	delete m_camera;
	Gizmos::destroy();
}

void MazeState::Update(double _dt) {
	m_camera->Update(_dt);
	m_maze->Update(_dt);
}
void MazeState::Draw() {
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	Gizmos::clear();
	glEnable(GL_DEPTH_TEST);

	m_maze->Draw(m_camera);

	Gizmos::draw(m_camera->GetProjectionView());
	DrawGUI();
}

void MazeState::DrawGUI() {
	if (ImGui::CollapsingHeader("Maze Generation")) {
		if (ImGui::TreeNode("General")) {
			if (ImGui::Button("Stop")) {
				m_maze->Stop();
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset")) {
				m_maze->ResetMaze();
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Random Traversal")) {
			if (ImGui::Button("Demonstrate")) {
				m_maze->ResetMaze();
				m_maze->DemonstrateRandomTraversal();
			}
			ImGui::SameLine();
			if (ImGui::Button("Instant")) {
				m_maze->ResetMaze();
				m_maze->RandomTraversal();
			}
			ImGui::TreePop();
		}
	}
}
