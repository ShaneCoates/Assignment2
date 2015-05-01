#include "BoardTile.h"
#include "Camera.h"
#include "aieutilities\Gizmos.h"
BoardTile::BoardTile() {

}
BoardTile::BoardTile(glm::vec2 _position) {
	m_position = _position;
	m_selected = false;
	m_open = false;
}
BoardTile::~BoardTile() {

}
void BoardTile::Update(double _dt) {
	m_colour = glm::vec4(0.3f, 0.3f, 0.3f, 1);
	if (m_type == eWhite) {
		m_colour = glm::vec4(1);
	}
}
void BoardTile::Draw(Camera* _camera) {
	if (m_selected) {
		m_colour = glm::vec4(0.5f, 0.5f, 1, 1);
	}
	if (m_open) {
		m_colour = glm::vec4(0.5f, 1, 0.5f, 1);
	}

	if (m_type == eBlackPiece) {
		Gizmos::addCylinderFilled(glm::vec3(m_position.x, 0.21f, m_position.y), 0.45f, 0.1f, 20, glm::vec4(0, 0, 0, 1));
	}
	else if (m_type == eRedPiece) {
		Gizmos::addCylinderFilled(glm::vec3(m_position.x, 0.21f, m_position.y), 0.45f, 0.1f, 20, glm::vec4(1, 0, 0, 1));
	}

	Gizmos::addAABBFilled(glm::vec3(m_position.x, 0, m_position.y), glm::vec3(0.5f, 0.1f, 0.5f), m_colour);
}
void BoardTile::Hover() {
	m_colour = glm::vec4(0, 0.7f, 0.7f, 1);
}
void BoardTile::Press(BoardTile* _tiles[8][8]) {
	m_selected = true;
	if (m_position.x > 0) {
		if (m_type == eBlackPiece && m_position.y > 0) {
			BoardTile* pTile = _tiles[(int)m_position.x - 1][(int)m_position.y - 1];
			if (pTile->m_type == eEmpty) {
				pTile->Highlight();
			}
			if (pTile->m_type == eRedPiece) {
				if (m_position.x > 1 && m_position.y > 1) {
					BoardTile* pNextTile = _tiles[(int)m_position.x - 2][(int)m_position.y - 2];
					if (pNextTile->m_type == eEmpty) {
						pNextTile->Highlight();
					}
				}
			}
		}
		else if (m_type == eRedPiece && m_position.y < 7) {
			BoardTile* pTile = _tiles[(int)m_position.x - 1][(int)m_position.y + 1];
			if (pTile->m_type == eEmpty) {
				pTile->Highlight();
			}
			if (pTile->m_type == eBlackPiece) {
				if (m_position.x > 1 && m_position.y < 6) {
					BoardTile* pNextTile = _tiles[(int)m_position.x - 2][(int)m_position.y + 2];
					if (pNextTile->m_type == eEmpty) {
						pNextTile->Highlight();
					}
				}
			}
		}
	}
	if (m_position.x < 7) {
		if (m_type == eBlackPiece && m_position.y > 0) {
			BoardTile* pTile = _tiles[(int)m_position.x + 1][(int)m_position.y - 1];
			if (pTile->m_type == eEmpty) {
				pTile->Highlight();
			}
			if (pTile->m_type == eRedPiece) {
				if (m_position.x < 6 && m_position.y > 1) {
					BoardTile* pNextTile = _tiles[(int)m_position.x + 2][(int)m_position.y - 2];
					if (pNextTile->m_type == eEmpty) {
						pNextTile->Highlight();
					}
				}
			}
		}
		else if (m_type == eRedPiece && m_position.y < 7) {
			BoardTile* pTile = _tiles[(int)m_position.x + 1][(int)m_position.y + 1];
			if (pTile->m_type == eEmpty) {
				pTile->Highlight();
			}
			if (pTile->m_type == eBlackPiece) {
				if (m_position.x < 6 && m_position.y < 6) {
					BoardTile* pNextTile = _tiles[(int)m_position.x + 2][(int)m_position.y + 2];
					if (pNextTile->m_type == eEmpty) {
						pNextTile->Highlight();
					}
				}
			}
		}
	}
	
}
void BoardTile::Highlight() {
	m_open = true;
}
void BoardTile::Deselect() {
	m_selected = false;
	m_open = false;
}