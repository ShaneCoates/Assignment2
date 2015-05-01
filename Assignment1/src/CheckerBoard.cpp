#include <GLFW\glfw3.h>
#include "CheckerBoard.h"
#include "Camera.h"
#include "aieutilities\Gizmos.h"
#include "BoardTile.h"
#include "NetworkManager.h"
CheckerBoard::CheckerBoard() {
	bool isWhite = false;
	for (int x = 0; x < 8; x++) {
		for (int z = 0; z < 8; z++) {
			m_tiles[x][z] = new BoardTile(glm::vec2(x, z));
			m_tiles[x][z]->m_type= (isWhite) ? eWhite : eEmpty;
			if (z < 3 && m_tiles[x][z]->m_type == eEmpty) {
				m_tiles[x][z]->m_type = eRedPiece;
			}
			else if (z > 4 && m_tiles[x][z]->m_type == eEmpty) {
				m_tiles[x][z]->m_type = eBlackPiece;
			}
			isWhite = !isWhite;
		}
		isWhite = !isWhite;
	}
	m_networkManagerInitialised = false;
}
CheckerBoard::~CheckerBoard() {

}
void CheckerBoard::Update(double _dt) {
	glm::vec2 pFrom, pTo;
	if (m_networkManagerInitialised) {
		if (m_networkManager->HasMoved(pFrom, pTo)) {
			Move(pFrom, pTo);
		}
	}
	for (int x = 0; x < 8; x++) {
		for (int z = 0; z < 8; z++) {
			m_tiles[x][z]->Update(_dt);
		}
	}
	double xpos, ypos;
	glfwGetCursorPos(glfwGetCurrentContext(), &xpos, &ypos);
	glm::vec3 pos = m_camera->PickAgainstPlane(xpos, ypos, glm::vec4(0, 1, 0, 0));
	if (glfwGetMouseButton(glfwGetCurrentContext(), GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
		if (!m_mouseDown) {
			m_mouseDown = true;
			bool success = false;
			for (int x = 0; x < 8; x++) {
				for (int z = 0; z < 8; z++) {
					if (pos.x < x + 0.5f && pos.x > x - 0.5f &&
						pos.z < z + 0.5f && pos.z > z - 0.5f) {
						if (m_tiles[x][z]->IsOpen()) {
							m_networkManager->SendMove(m_selectedTile, glm::vec2(x, z));
						}
						else {
							m_tiles[x][z]->Press(m_tiles);
							m_selectedTile = glm::vec2(x, z);
							success == true;
						}
					}
				}
			}
		}
		/*if (!success) {
			for (int x = 0; x < 8; x++) {
				for (int z = 0; z < 8; z++) {
					m_tiles[x][z]->Deselect();
				}
			}
		}*/
	}
	else {
		m_mouseDown = false;
		for (int x = 0; x < 8; x++) {
			for (int z = 0; z < 8; z++) {
				if (pos.x < x + 0.5f && pos.x > x - 0.5f &&
					pos.z < z + 0.5f && pos.z > z - 0.5f) {
					m_tiles[x][z]->Hover();
				}
			}
		}
	}
}
void CheckerBoard::Draw() {
	glm::vec4 white(1);
	glm::vec4 black(0.3f, 0.3f, 0.3f, 1);
	
	for (int x = 0; x < 8; x++) {
		for (int z = 0; z < 8; z++) {
			m_tiles[x][z]->Draw(m_camera);
		}
	}

	Gizmos::draw(m_camera->GetProjectionView());
}

void CheckerBoard::Move(glm::vec2 _from, glm::vec2 _to) {

	m_tiles[(int)_to.x][(int)_to.y]->m_type = m_tiles[(int)_from.x][(int)_from.y]->m_type;
	m_tiles[(int)_from.x][(int)_from.y]->m_type = eEmpty;
	m_tiles[(int)_to.x][(int)_to.y]->Deselect();
	m_tiles[(int)_from.x][(int)_from.y]->Deselect();
}

void CheckerBoard::SetCamera(Camera* _camera) {
	m_camera = _camera;
}