#include "TestState.h"
#include "FlyCamera.h"
#include "ShaderLoader.h"
#include <stdio.h>
#include "RakPeerInterface.h"
#include "NetworkManager.h"
#include "imgui.h"
#define IM_ARRAYSIZE(_ARR)          ((int)(sizeof(_ARR)/sizeof(*_ARR)))

void TestState::Init(GLFWwindow* _window, GameStateManager* _gameStateManager) {
	m_window = _window;
	m_gameStateManager = _gameStateManager;
	m_camera = new FlyCamera(10.0f);
	m_camera->SetInputWindow(m_window);
	m_camera->SetPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 1000.0f);
	m_camera->SetLookAt(glm::vec3(100, 20, 100), glm::vec3(0), glm::vec3(0, 1, 0));

	m_networkServer = new NetworkManager();
	m_networkClient = new NetworkManager();

	m_clientName = DEFAULT_NAME;
	m_serverIP = DEFAULT_IP;
	m_serverPort = DEFAULT_PORT;

	glfwSetWindowCloseCallback(m_window, *WindowClose(m_window));

}

TestState::~TestState() {
	delete m_camera;
	m_networkClient->TerminateConnection();
	m_networkServer->TerminateConnection();
	delete m_networkClient;
	delete m_networkServer;
}

void TestState::Update(double _dt) {
	m_camera->Update(_dt);

	if (m_networkClient->IsInitialized()) {
		if (ImGui::GetConsoleUpdated()) {
			m_networkClient->Send(ImGui::GetConsoleBuffer());
		}
		m_networkClient->Update(_dt);
	} else if (m_networkServer->IsInitialized()) {
		if (ImGui::GetConsoleUpdated()) {
			m_networkServer->Send(ImGui::GetConsoleBuffer());
		}
		m_networkServer->Update(_dt);
	}
	
	if (glfwGetKey(m_window, GLFW_KEY_P) == GLFW_PRESS)  {
		if (m_networkClient->IsInitialized())
			m_networkClient->Send("C4");
		else if (m_networkServer->IsInitialized())
			m_networkServer->Send("C5");
	}

	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)  {
		WindowClose(m_window);
	}
}
void TestState::Draw() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	if (ImGui::TreeNode("Network Options"))
	{
		if (ImGui::Button("Start Server"))
		{
			if (!m_networkServer->IsInitialized())
			{
				m_networkServer->InitializeServer(m_serverPort, (char*)m_serverIP.c_str());
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Start Client"))
		{
			if (!m_networkClient->IsInitialized())
			{
				if (!m_clientName.empty())
					m_networkClient->InitializeClient((char*)m_clientName.c_str(), m_serverPort, (char*)m_serverIP.c_str());
				else
					m_networkClient->InitializeClient(DEFAULT_NAME, m_serverPort, (char*)m_serverIP.c_str());
			}
		}

		ImGui::InputText("Player Name", (char*)m_clientName.c_str(), 256);
		ImGui::InputText("Server IP", (char*)m_serverIP.c_str(), 64);
		ImGui::InputInt("Server Port", (int*)&m_serverPort);

		bool drawList = false;
		const char* listbox_items[MAX_CLIENTS];
		static int listbox_item_current = 0;
		if (m_networkClient->IsInitialized()) {
			for (int i = 0; i < MAX_CLIENTS; ++i) {
				listbox_items[i] = m_networkClient->m_clients[i];
				drawList = true;
			}
		} else if (m_networkServer->IsInitialized()) {
			for (int i = 0; i < MAX_CLIENTS; ++i) {
				listbox_items[i] = m_networkServer->m_clients[i];
				drawList = true;
			}
		}
		if (drawList) {
			ImGui::ListBox("Clients", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 4);
		}
		ImGui::TreePop();
	}
	ImGui::ShowCustomConsole();
}

GLFWwindowclosefun TestState::WindowClose(GLFWwindow* window) {
	m_networkClient->TerminateConnection();
	m_networkServer->TerminateConnection();
	return NULL;
}