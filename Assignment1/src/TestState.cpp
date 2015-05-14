#include "TestState.h"
#include "FlyCamera.h"
#include "ShaderLoader.h"
#include <stdio.h>
#include "RakPeerInterface.h"
#include "NetworkManager.h"
#include "imgui.h"
#include "aieutilities\Gizmos.h"
#include "CheckerBoard.h"
#include "Skybox.h"
#define IM_ARRAYSIZE(_ARR)          ((int)(sizeof(_ARR)/sizeof(*_ARR)))

void TestState::Init(GLFWwindow* _window, GameStateManager* _gameStateManager) {
	m_window = _window;
	m_gameStateManager = _gameStateManager;
	m_camera = new FlyCamera(10.0f);
	m_camera->SetInputWindow(m_window);
	m_camera->SetPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 1000.0f);
	m_camera->SetLookAt(glm::vec3(4, 10, 14), glm::vec3(4, 0, 4), glm::vec3(0, 1, 0));

	m_networkServer = new NetworkManager();
	m_networkClient = new NetworkManager();

	m_clientName = DEFAULT_NAME;
	m_serverIP = DEFAULT_IP;
	m_serverPort = DEFAULT_PORT;

	m_checkerBoard = new CheckerBoard();
	m_checkerBoard->SetCamera(m_camera);
	m_skybox = new Skybox();

	Gizmos::create();
}

TestState::~TestState() {
	delete m_camera;
	m_networkClient->TerminateConnection();
	m_networkServer->TerminateConnection();
	delete m_networkClient;
	delete m_networkServer;
	Gizmos::destroy();
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
	m_checkerBoard->Update(_dt);
}
void TestState::Draw() {
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	Gizmos::clear();

	glEnable(GL_DEPTH_TEST);
	if (m_networkClient->IsInitialized()) {
		m_checkerBoard->Draw();
	}
	DrawGUI();
}

void TestState::DrawGUI() {
	if (ImGui::CollapsingHeader("Network Options")) {
		ImGui::InputText("Player Name", (char*)m_clientName.c_str(), 256);
		ImGui::InputText("Server IP", (char*)m_serverIP.c_str(), 64);
		ImGui::InputInt("Server Port", (int*)&m_serverPort);
		if (ImGui::Button("Start Server")) {
			if (!m_networkServer->IsInitialized()) {
				m_networkServer->InitializeServer(m_serverPort, (char*)m_serverIP.c_str());
				m_checkerBoard->SetNetworkManager(m_networkServer);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Start Client")) {
			if (!m_networkClient->IsInitialized()) {
				if (!m_clientName.empty()) {
					m_networkClient->InitializeClient((char*)m_clientName.c_str(), m_serverPort, (char*)m_serverIP.c_str());
				}
				else {
					m_networkClient->InitializeClient(DEFAULT_NAME, m_serverPort, (char*)m_serverIP.c_str());
				}
				m_checkerBoard->SetNetworkManager(m_networkClient);
			}
		}
	}

	bool drawList = false;
	const char* listbox_items[MAX_CLIENTS];
	static int listbox_item_current = 0;
	if (m_networkClient->IsInitialized() && m_networkClient->m_clients.size() > 0) {
		std::copy(m_networkClient->m_clients.begin(), m_networkClient->m_clients.end(), listbox_items);
		ImGui::ListBox("Clients", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 4);
	}
	else if (m_networkServer->IsInitialized() && m_networkServer->m_clients.size() > 0) {
		std::copy(m_networkServer->m_clients.begin(), m_networkServer->m_clients.end(), listbox_items);
		ImGui::ListBox("Clients", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 4);
	}

	ImGui::ShowCustomConsole();
}
