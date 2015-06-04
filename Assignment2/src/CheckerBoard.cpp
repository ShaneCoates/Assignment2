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
					if (m_blackTurn && mouseOver->m_type == eBlackPiece) {
						DeselectAll();
						if (m_availableTakes.size() > 0) {
							bool found = false;
							for (unsigned int i = 0; i < m_availableTakes.size(); i++) {
								if (m_availableTakes[i].From == mouseOver->m_position){
									mouseOver->Press(m_tiles);
									m_selectedTile = glm::vec2(mouseOver->m_position.x, mouseOver->m_position.y);
									found = true;
								}
							}
							if (!found) {
								for (unsigned int i = 0; i < m_availableTakes.size(); i++) {
									m_tiles[(int)m_availableTakes[i].From.x][(int)m_availableTakes[i].From.y]->Flash();
									m_tiles[(int)m_availableTakes[i].To.x][(int)m_availableTakes[i].To.y]->Flash();
								}
							}
						}
						else {
							mouseOver->Press(m_tiles);
							m_selectedTile = glm::vec2(mouseOver->m_position.x, mouseOver->m_position.y);
						}
					}
				}
				else {
					if (!m_blackTurn && mouseOver->m_type == eRedPiece) {
						DeselectAll();
						if (m_availableTakes.size() > 0) {
							bool found = false;
							for (unsigned int i = 0; i < m_availableTakes.size(); i++) {
								if (m_availableTakes[i].From == mouseOver->m_position){
									mouseOver->Press(m_tiles);
									m_selectedTile = glm::vec2(mouseOver->m_position.x, mouseOver->m_position.y);
									found = true;
								}
							}
							if (!found) {
								for (unsigned int i = 0; i < m_availableTakes.size(); i++) {
									m_tiles[(int)m_availableTakes[i].From.x][(int)m_availableTakes[i].From.y]->Flash();
									m_tiles[(int)m_availableTakes[i].To.x][(int)m_availableTakes[i].To.y]->Flash();
								}
							}
						}
						else {
							mouseOver->Press(m_tiles);
							m_selectedTile = glm::vec2(mouseOver->m_position.x, mouseOver->m_position.y);
						}
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
	bool wasTake = false;
	m_tiles[(int)_to.x][(int)_to.y]->m_type = m_tiles[(int)_from.x][(int)_from.y]->m_type;
	m_tiles[(int)_to.x][(int)_to.y]->m_king = m_tiles[(int)_from.x][(int)_from.y]->m_king;

	m_tiles[(int)_from.x][(int)_from.y]->m_type = eEmpty;
	m_tiles[(int)_from.x][(int)_from.y]->m_king = false;

	if (abs((int)_to.x - (int)_from.x) == 2){
		m_tiles[(int)((_from.x + _to.x) * 0.5f)][(int)((_from.y + _to.y) * 0.5f)]->m_type = eEmpty;
		wasTake = true;
	}
	CheckGameOver();
	if (m_tiles[(int)_to.x][(int)_to.y]->CanTake(m_tiles) && wasTake) {
		//do nothing 
	} else {
		m_blackTurn = !m_blackTurn;
	}
	CheckForTakeAvailable();
}

void CheckerBoard::CheckGameOver() {
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
void CheckerBoard::CheckForTakeAvailable() {
	m_availableTakes.clear();
	if (m_blackTurn) {
		for (int x = 0; x < 8; x++) {
			for (int z = 0; z < 8; z++) {
				BoardTile* currentTile = m_tiles[x][z];
				PossibleMove move;
				move.From = glm::vec2(x, z);
				if (currentTile->m_type == eBlackPiece) {
					if (z > 1) {
						if (x < 6) {
							if (m_tiles[x + 2][z - 2]->m_type == eEmpty &&
								m_tiles[x + 1][z - 1]->m_type == eRedPiece) {
								move.To = glm::vec2(x + 2, z - 2);
								m_availableTakes.push_back(move);
							}
						}
						if (x > 1) {
							if (m_tiles[x - 2][z - 2]->m_type == eEmpty &&
								m_tiles[x - 1][z - 1]->m_type == eRedPiece) {
								move.To = glm::vec2(x - 2, z - 2);
								m_availableTakes.push_back(move);
							}
						}
					}
				}
				if (currentTile->m_king) {
					if (z < 6) {
						if (x < 6) {
							if (m_tiles[x + 2][z + 2]->m_type == eEmpty &&
								m_tiles[x + 1][z + 1]->m_type == eRedPiece) {
								move.To = glm::vec2(x + 2, z + 2);
								m_availableTakes.push_back(move);
							}
						}
						if (x > 1) {
							if (m_tiles[x - 2][z + 2]->m_type == eEmpty &&
								m_tiles[x - 1][z + 1]->m_type == eRedPiece) {
								move.To = glm::vec2(x - 2, z + 2);
								m_availableTakes.push_back(move);
							}
						}
					}
				}
			}
		}
	}
	else {
		for (int x = 0; x < 8; x++) {
			for (int z = 0; z < 8; z++) {
				BoardTile* currentTile = m_tiles[x][z];
				PossibleMove move;
				move.From = glm::vec2(x, z);
				if (currentTile->m_type == eRedPiece) {
					if (z < 6) {
						if (x < 6) {
							if (m_tiles[x + 2][z + 2]->m_type == eEmpty &&
								m_tiles[x + 1][z + 1]->m_type == eBlackPiece) {
								m_availableTakes.push_back(move);
								m_availableTakes.back().To = glm::vec2(x + 2, z + 2);
							}
						}
						if (x > 1) {
							if (m_tiles[x - 2][z + 2]->m_type == eEmpty &&
								m_tiles[x - 1][z + 1]->m_type == eBlackPiece) {
								m_availableTakes.push_back(move);
								m_availableTakes.back().To = glm::vec2(x - 2, z + 2);
							}
						}
					}
					if (currentTile->m_king && z > 1) {
						if (x < 6) {
							if (m_tiles[x + 2][z - 2]->m_type == eEmpty &&
								m_tiles[x + 1][z - 1]->m_type == eBlackPiece) {
								m_availableTakes.push_back(move);
								m_availableTakes.back().To = glm::vec2(x + 2, z - 2);
							}
						}
						if (x > 1) {
							if (m_tiles[x - 2][z - 2]->m_type == eEmpty &&
								m_tiles[x - 1][z - 1]->m_type == eBlackPiece) {
								m_availableTakes.push_back(move);
								m_availableTakes.back().To = glm::vec2(x - 2, z - 2);
							}
						}
					}
				}
			}
		}
	}
	
}

bool CheckerBoard::TakeAvailable() {
	if (m_availableTakes.size() > 0) {
		return true;
	}
	return false;
}
void CheckerBoard::DeselectAll() {
	for (unsigned int x = 0; x < 8; x++) {
		for (unsigned int z = 0; z < 8; z++) {
			m_tiles[x][z]->Deselect();
		}
	}
}