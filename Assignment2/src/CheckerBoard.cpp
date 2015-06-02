#include <GLFW\glfw3.h>
#include "CheckerBoard.h"
#include "Camera.h"
#include "aieutilities\Gizmos.h"
#include "BoardTile.h"
#include "NetworkManager.h"
CheckerBoard::CheckerBoard() {
	m_networkManagerInitialised = false;
	GameOver();
}
CheckerBoard::~CheckerBoard() {

}
void CheckerBoard::Update(double _dt) {
	CheckForMoves();
	for (int x = 0; x < 8; x++) {
		for (int z = 0; z < 8; z++) {
			m_tiles[x][z]->Update(_dt);
		}
	}
	BoardTile* mouseOver = GetMouseOver();
	if (glfwGetMouseButton(glfwGetCurrentContext(), GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
		if (!m_mouseDown) {
			m_mouseDown = true;
			if (mouseOver->IsOpen() && mouseOver->m_type == eEmpty) {
				m_networkManager->SendMove(m_selectedTile, glm::vec2(mouseOver->m_position.x, mouseOver->m_position.y));
			}
			else if (mouseOver->m_type != eWhite) {
				if (m_controllingBlack) {
					if (m_blackTurn && mouseOver->m_type == eBlackPiece ) {
						mouseOver->Press(m_tiles);
						m_selectedTile = glm::vec2(mouseOver->m_position.x, mouseOver->m_position.y);
					}
				}
				else {
					if (!m_blackTurn && mouseOver->m_type == eRedPiece) {
						mouseOver->Press(m_tiles);
						m_selectedTile = glm::vec2(mouseOver->m_position.x, mouseOver->m_position.y);
					}
				}
				
			}
		}
	}
	else {
		m_mouseDown = false;
		mouseOver->Hover();
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
	Gizmos::addAABBFilled(glm::vec3(3.5f, 0, 3.5f), glm::vec3(4.5f, 0.099f, 4.5f), glm::vec4(160.0f / 255.0f, 82.0f / 255.0f, 45.0f / 255.0f, 1.0f));
	Gizmos::draw(m_camera->GetProjectionView());
}

BoardTile* CheckerBoard::GetMouseOver() {
	double xpos, ypos;
	glfwGetCursorPos(glfwGetCurrentContext(), &xpos, &ypos);
	glm::vec3 pos = m_camera->PickAgainstPlane((float)xpos, (float)ypos, glm::vec4(0, 1, 0, 0));
	for (int x = 0; x < 8; x++) {
		for (int z = 0; z < 8; z++) {
			if (pos.x < x + 0.5f && pos.x > x - 0.5f &&
				pos.z < z + 0.5f && pos.z > z - 0.5f) {
				return m_tiles[x][z];
			}
		}
	}
	return new BoardTile;
}

void CheckerBoard::CheckForMoves() {
	if (m_networkManagerInitialised) {
		glm::vec2 pFrom, pTo;
		if (m_networkManager->HasMoved(pFrom, pTo)) {
			Move(pFrom, pTo);
		}
	}
}

void CheckerBoard::Move(glm::vec2 _from, glm::vec2 _to) {

	m_tiles[(int)_to.x][(int)_to.y]->m_type = m_tiles[(int)_from.x][(int)_from.y]->m_type;
	m_tiles[(int)_to.x][(int)_to.y]->m_king = m_tiles[(int)_from.x][(int)_from.y]->m_king;

	m_tiles[(int)_from.x][(int)_from.y]->m_type = eEmpty;
	m_tiles[(int)_from.x][(int)_from.y]->m_king = false;

	if (abs((int)_to.x - (int)_from.x) == 2){
		m_tiles[(int)((_from.x + _to.x) * 0.5f)][(int)((_from.y + _to.y) * 0.5f)]->m_type = eEmpty;
	}
	bool blackFound, redFound = false;
	
	for (int x = 0; x < 8; x++) {
		for (int z = 0; z < 8; z++) {
			m_tiles[x][z]->Deselect();
			if (m_tiles[x][z]->m_type == eBlackPiece) {
				if (z == 0) {
					m_tiles[x][z]->m_king = true;
				}
				blackFound = true;
			}
			if (m_tiles[x][z]->m_type == eRedPiece) {
				if (z == 7) {
					m_tiles[x][z]->m_king = true;
				}
				redFound = true;
			}
		}
	}
	if (!redFound || !blackFound) {
		GameOver();
	}
	m_blackTurn = !m_blackTurn;
}

void CheckerBoard::GameOver() {
	bool isWhite = false;
	for (int x = 0; x < 8; x++) {
		for (int z = 0; z < 8; z++) {
			m_tiles[x][z] = new BoardTile(glm::vec2(x, z));
			m_tiles[x][z]->m_type = (isWhite) ? eWhite : eEmpty;
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
	m_blackTurn = true;
}

void CheckerBoard::SetCamera(Camera* _camera) {
	m_camera = _camera;
}