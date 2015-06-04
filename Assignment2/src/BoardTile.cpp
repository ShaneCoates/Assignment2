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
	m_colour = glm::vec4(0.3f, 0.3f, 0.3f, 1);
	m_flashTimer = 0;
	m_flash = false;
	m_glowTimer = 0;
}
BoardTile::~BoardTile() {

}
void BoardTile::Update(double _dt) {
	m_glowTimer += _dt;
	m_colour = glm::vec4(0.3f, 0.3f, 0.3f, 1);
	if (m_flash) {
		m_flashTimer -= _dt;
	}
	if (m_flashTimer <= 0) {
		m_flash = false;
	}
}
void BoardTile::Draw(Camera* _camera) {
	if (m_type == eWhite) {
		m_colour = glm::vec4(1);
	}
	if (m_selected) {
		m_colour = glm::vec4(0.5f, 0.5f, (sin(m_glowTimer) * 0.5f + 0.5f), 1);
	}
	if (m_open) {
		m_colour = glm::vec4(0.5f, (sin(m_glowTimer) * 0.5f + 0.5f), 0.5f, 1);
	}
	if (m_flash) {
		m_colour = glm::vec4(1, 0.5f, 0.5f, 1);
	}
	if (m_type == eBlackPiece || m_type == eRedPiece) {
		glm::vec4 pieceColour;
		float alpha = 1;
		if (m_selected) alpha = (sin(m_glowTimer) * 0.25f) + 0.5f;
		if (m_type == eBlackPiece) {
			pieceColour = glm::vec4(0, 0, 0, alpha);
		}
		else if (m_type == eRedPiece) {
			pieceColour = glm::vec4(1, 0, 0, alpha);
		}
		Gizmos::addCylinderFilled(glm::vec3(m_position.x, 0.21f, m_position.y), 0.4f, 0.05f, 50, pieceColour);
		if (m_king) {
			Gizmos::addCylinderFilled(glm::vec3(m_position.x, 0.31f, m_position.y), 0.4f, 0.05f, 50, pieceColour);
		}
	}

	Gizmos::addAABBFilled(glm::vec3(m_position.x, 0, m_position.y), glm::vec3(0.5f, 0.1f, 0.5f), m_colour);
}
void BoardTile::Hover() {
	m_colour = glm::vec4(0, 0.7f, 0.7f, 1);
}
void BoardTile::Press(BoardTile* _tiles[8][8]) {
	m_selected = true;
	bool canTake = false;
	if (m_position.x > 0) {
		if ((m_type == eBlackPiece || (m_type == eRedPiece && m_king)) && m_position.y > 0) {
			BoardTile* pTile = _tiles[(int)m_position.x - 1][(int)m_position.y - 1];
			if ((pTile->m_type == eRedPiece && m_type == eBlackPiece) ||
				(pTile->m_type == eBlackPiece && m_type == eRedPiece)) {
				if (m_position.x > 1 && m_position.y > 1) {
					BoardTile* pNextTile = _tiles[(int)m_position.x - 2][(int)m_position.y - 2];
					if (pNextTile->m_type == eEmpty) {
						pNextTile->Highlight();
						canTake = true;
					}
				}
			}
		}
		if ((m_type == eRedPiece || (m_type == eBlackPiece && m_king)) && m_position.y < 7) {
			BoardTile* pTile = _tiles[(int)m_position.x - 1][(int)m_position.y + 1];
			if ((pTile->m_type == eRedPiece && m_type == eBlackPiece) ||
				(pTile->m_type == eBlackPiece && m_type == eRedPiece)) {
				if (m_position.x > 1 && m_position.y < 6) {
					BoardTile* pNextTile = _tiles[(int)m_position.x - 2][(int)m_position.y + 2];
					if (pNextTile->m_type == eEmpty) {
						pNextTile->Highlight(); 
						canTake = true;
					}
				}
			}
		}
	}
	if (m_position.x < 7) {
		if ((m_type == eBlackPiece || (m_type == eRedPiece && m_king)) && m_position.y > 0) {
			BoardTile* pTile = _tiles[(int)m_position.x + 1][(int)m_position.y - 1];
			if ((pTile->m_type == eRedPiece && m_type == eBlackPiece) ||
				(pTile->m_type == eBlackPiece && m_type == eRedPiece)) {
				if (m_position.x < 6 && m_position.y > 1) {
					BoardTile* pNextTile = _tiles[(int)m_position.x + 2][(int)m_position.y - 2];
					if (pNextTile->m_type == eEmpty) {
						pNextTile->Highlight();
						canTake = true;
					}
				}
			}
		}
		if ((m_type == eRedPiece || (m_type == eBlackPiece && m_king)) && m_position.y < 7) {
			BoardTile* pTile = _tiles[(int)m_position.x + 1][(int)m_position.y + 1];
			if ((pTile->m_type == eRedPiece && m_type == eBlackPiece) ||
				(pTile->m_type == eBlackPiece && m_type == eRedPiece)) {
				if (m_position.x < 6 && m_position.y < 6) {
					BoardTile* pNextTile = _tiles[(int)m_position.x + 2][(int)m_position.y + 2];
					if (pNextTile->m_type == eEmpty) {
						pNextTile->Highlight();
						canTake = true;
					}
				}
			}
		}
	}

	if (!canTake) {
		if (m_position.x > 0) {
			if ((m_type == eBlackPiece || (m_type == eRedPiece && m_king)) && m_position.y > 0) {
				BoardTile* pTile = _tiles[(int)m_position.x - 1][(int)m_position.y - 1];
				if (pTile->m_type == eEmpty) {
					pTile->Highlight();
				}
			}
			if ((m_type == eRedPiece || (m_type == eBlackPiece && m_king)) && m_position.y < 7) {
				BoardTile* pTile = _tiles[(int)m_position.x - 1][(int)m_position.y + 1];
				if (pTile->m_type == eEmpty) {
					pTile->Highlight();
				}
			}
		}
		if (m_position.x < 7) {
			if ((m_type == eBlackPiece || (m_type == eRedPiece && m_king)) && m_position.y > 0) {
				BoardTile* pTile = _tiles[(int)m_position.x + 1][(int)m_position.y - 1];
				if (pTile->m_type == eEmpty) {
					pTile->Highlight();
				}
			}
			if ((m_type == eRedPiece || (m_type == eBlackPiece && m_king)) && m_position.y < 7) {
				BoardTile* pTile = _tiles[(int)m_position.x + 1][(int)m_position.y + 1];
				if (pTile->m_type == eEmpty) {
					pTile->Highlight();
				}
			}
		}
	}
	
}
void BoardTile::Highlight() {
	m_open = true;
}
void BoardTile::Flash() {
	m_flash = true;
	m_flashTimer = 0.2;
}
void BoardTile::Deselect() {
	m_selected = false;
	m_open = false;
}
bool BoardTile::CanTake(BoardTile* _tiles[8][8]) {
	bool found = false;
	if (m_position.x > 0) {
		if ((m_type == eBlackPiece || (m_type == eRedPiece && m_king)) && m_position.y > 0) {
			BoardTile* pTile = _tiles[(int)m_position.x - 1][(int)m_position.y - 1];
			if ((pTile->m_type == eRedPiece && m_type == eBlackPiece) ||
				(pTile->m_type == eBlackPiece && m_type == eRedPiece)) {
				if (m_position.x > 1 && m_position.y > 1) {
					BoardTile* pNextTile = _tiles[(int)m_position.x - 2][(int)m_position.y - 2];
					if (pNextTile->m_type == eEmpty) {
						pNextTile->Highlight();
						found = true;
					}
				}
			}
		}
		if ((m_type == eRedPiece || (m_type == eBlackPiece && m_king)) && m_position.y < 7) {
			BoardTile* pTile = _tiles[(int)m_position.x - 1][(int)m_position.y + 1];
			if ((pTile->m_type == eRedPiece && m_type == eBlackPiece) ||
				(pTile->m_type == eBlackPiece && m_type == eRedPiece)) {
				if (m_position.x > 1 && m_position.y < 6) {
					BoardTile* pNextTile = _tiles[(int)m_position.x - 2][(int)m_position.y + 2];
					if (pNextTile->m_type == eEmpty) {
						pNextTile->Highlight();
						found = true;
					}
				}
			}
		}
	}
	if (m_position.x < 7) {
		if ((m_type == eBlackPiece || (m_type == eRedPiece && m_king)) && m_position.y > 0) {
			BoardTile* pTile = _tiles[(int)m_position.x + 1][(int)m_position.y - 1];
			if ((pTile->m_type == eRedPiece && m_type == eBlackPiece) ||
				(pTile->m_type == eBlackPiece && m_type == eRedPiece)) {
				if (m_position.x < 6 && m_position.y > 1) {
					BoardTile* pNextTile = _tiles[(int)m_position.x + 2][(int)m_position.y - 2];
					if (pNextTile->m_type == eEmpty) {
						pNextTile->Highlight();
						found = true;
					}
				}
			}
		}
		if ((m_type == eRedPiece || (m_type == eBlackPiece && m_king)) && m_position.y < 7) {
			BoardTile* pTile = _tiles[(int)m_position.x + 1][(int)m_position.y + 1];
			if ((pTile->m_type == eRedPiece && m_type == eBlackPiece) ||
				(pTile->m_type == eBlackPiece && m_type == eRedPiece)) {
				if (m_position.x < 6 && m_position.y < 6) {
					BoardTile* pNextTile = _tiles[(int)m_position.x + 2][(int)m_position.y + 2];
					if (pNextTile->m_type == eEmpty) {
						pNextTile->Highlight();
						found = true;
					}
				}
			}
		}
	}
	return found;
}