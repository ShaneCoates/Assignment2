#include "BoardTile.h"
#include "Camera.h"
#include "aieutilities\Gizmos.h"
BoardTile::BoardTile() {

}
BoardTile::BoardTile(glm::vec2 _position) {
	m_position = _position;
	m_selected = false;
	m_open = false;
	m_king = false;
}
BoardTile::~BoardTile() {

}
void BoardTile::Update(double _dt) {

}
void BoardTile::Draw(Camera* _camera) {
	m_colour = glm::vec4(0.3f, 0.3f, 0.3f, 1);
	if (m_type == eWhite) {
		m_colour = glm::vec4(1);
	}
	if (m_selected) {
		m_colour = glm::vec4(0.5f, 0.5f, 1, 1);
	}
	if (m_open) {
		m_colour = glm::vec4(0.5f, 1, 0.5f, 1);
	}
	if (m_type == eBlackPiece || m_type == eRedPiece) {
		glm::vec4 pieceColour;
		if (m_type == eBlackPiece) {
			pieceColour = glm::vec4(0, 0, 0, 1);
		}
		else if (m_type == eRedPiece) {
			pieceColour = glm::vec4(1, 0, 0, 1);
		}
		Gizmos::addCylinderFilled(glm::vec3(m_position.x, 0.21f, m_position.y), 0.4f, 0.05f, 20, pieceColour);
		if (m_king) {
			Gizmos::addCylinderFilled(glm::vec3(m_position.x, 0.31f, m_position.y), 0.4f, 0.05f, 20, pieceColour);
		}
	}

	Gizmos::addAABBFilled(glm::vec3(m_position.x, 0, m_position.y), glm::vec3(0.5f, 0.1f, 0.5f), m_colour);
}
void BoardTile::Hover() {
	m_colour = glm::vec4(0, 0.7f, 0.7f, 1);
}
void BoardTile::Press(BoardTile* _tiles[8][8]) {
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			_tiles[x][y]->Deselect();
		}
	}
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