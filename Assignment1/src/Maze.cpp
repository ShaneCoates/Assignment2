#include "Maze.h"
#include "aieutilities\Gizmos.h"
#include <list>
#include <vector>
#include <algorithm>
#include <iterator>

Maze::Maze() {
	for (unsigned int x = 0; x < MAZE_SIZE; x++) {
		for (unsigned int z = 0; z < MAZE_SIZE; z++) {
			m_mazePieces[x][z] = new MazePiece();
			m_mazePieces[x][z]->Position = glm::vec3(x, 0, z);
			m_mazePieces[x][z]->Wall = true;
			m_mazePieces[x][z]->Traversed = false;

		}
	}
	m_demonstratingRandomTraversal = false;
}
Maze::~Maze() {

}
void Maze::Update(double _dt) {
	if (m_demonstratingRandomTraversal) {
		DemonstrateRandomTraversal();
	}
}
void Maze::Draw(Camera* _camera) {
	Gizmos::addAABBFilled(glm::vec3((MAZE_SIZE * 0.5f) - 0.5f, -0.75f, (MAZE_SIZE * 0.5f) - 0.5f), glm::vec3((MAZE_SIZE * 0.5f) + 0.5f, 0.5f, (MAZE_SIZE * 0.5f) + 0.5f), glm::vec4(0, 0, 0, 1));
	for (unsigned int x = 0; x < MAZE_SIZE; x++) {
		for (unsigned int z = 0; z < MAZE_SIZE; z++) {
			MazePiece* mp = m_mazePieces[x][z];
			glm::vec4 pColour = glm::vec4(1);
			if (mp->Traversed) {
				pColour = glm::vec4(1, 0, 1, 1);
			}
			else {
				pColour = glm::vec4(1, 0, 0, 1);
			}
			if (mp->Wall) {
				Gizmos::addAABBFilled(glm::vec3(mp->Position.x, 0, mp->Position.z), glm::vec3(0.5f, 0.25f, 0.5f), pColour);
			}
		}
	}
}

void Maze::Stop() {
	m_demonstratingRandomTraversal = false;
	m_randomTraversalOpen.clear();
}

void Maze::ResetMaze() {
	for (unsigned int x = 0; x < MAZE_SIZE; x++) {
		for (unsigned int z = 0; z < MAZE_SIZE; z++) {
			m_mazePieces[x][z]->Position = glm::vec3(x, 0, z);
			m_mazePieces[x][z]->Wall = true;
			m_mazePieces[x][z]->Traversed = false;
		}
	}
	Stop();
}

void Maze::RandomTraversal() {
	std::list<MazePiece*> pOpen;
	pOpen.push_back(m_mazePieces[1][1]);
	m_mazePieces[1][1]->Traversed = true;
	while (pOpen.size() > 0) {
		std::vector<MazePiece*> m_tempVector{ std::begin(pOpen), std::end(pOpen) };
		std::random_shuffle(m_tempVector.begin(), m_tempVector.end());
		pOpen.clear();
		std::copy(m_tempVector.begin(), m_tempVector.end(), std::back_inserter(pOpen));

		pOpen.front()->Wall = false;

		glm::vec2 pos = pOpen.front()->Position.xz;
		if (pos.x > 1) {
			MazePiece* nextPiece = m_mazePieces[(int)pos.x - 1][(int)pos.y];
			if (!nextPiece->Traversed) {
				nextPiece->Traversed = true;
				if (m_mazePieces[(int)pos.x - 2][(int)pos.y]->Wall) {
					if (!(std::find(pOpen.begin(), pOpen.end(), nextPiece) != pOpen.end())) {
						pOpen.push_back(nextPiece);
					}
				}
			}
			else {
				pOpen.remove(nextPiece);
			}
		}
		if (pos.x < MAZE_SIZE - 2) {
			MazePiece* nextPiece = m_mazePieces[(int)pos.x + 1][(int)pos.y];
			if (!nextPiece->Traversed) {
				nextPiece->Traversed = true;
				if (m_mazePieces[(int)pos.x + 2][(int)pos.y]->Wall) {
					if (!(std::find(pOpen.begin(), pOpen.end(), nextPiece) != pOpen.end())) {
						pOpen.push_back(nextPiece);
					}
				}
			}
			else {
				pOpen.remove(nextPiece);
			}
		}
		if (pos.y > 1) {
			MazePiece* nextPiece = m_mazePieces[(int)pos.x][(int)pos.y - 1];
			if (!nextPiece->Traversed) {
				nextPiece->Traversed = true;
				if (m_mazePieces[(int)pos.x][(int)pos.y - 2]->Wall) {
					if (!(std::find(pOpen.begin(), pOpen.end(), nextPiece) != pOpen.end())) {
						pOpen.push_back(nextPiece);
					}
				}
			}
			else {
				pOpen.remove(nextPiece);
			}
		}
		if (pos.y < MAZE_SIZE - 2) {
			MazePiece* nextPiece = m_mazePieces[(int)pos.x][(int)pos.y + 1];
			if (!nextPiece->Traversed) {
				nextPiece->Traversed = true;
				if (m_mazePieces[(int)pos.x][(int)pos.y + 2]->Wall) {
					if (!(std::find(pOpen.begin(), pOpen.end(), nextPiece) != pOpen.end())) {
						pOpen.push_back(nextPiece);
					}
				}
			}
			else {
				pOpen.remove(nextPiece);
			}
		}
		pOpen.pop_front();

	}
	m_mazePieces[0][1]->Wall = false;

	m_mazePieces[MAZE_SIZE - 2][MAZE_SIZE - 1]->Wall = false;
	m_mazePieces[MAZE_SIZE - 2][MAZE_SIZE - 2]->Wall = false;
	for (unsigned int x = 0; x < MAZE_SIZE; x++) {
		for (unsigned int z = 0; z < MAZE_SIZE; z++) {
			m_mazePieces[x][z]->Traversed = false;
		}
	}

}

void Maze::DemonstrateRandomTraversal() {
	if (!m_demonstratingRandomTraversal) {
		m_randomTraversalOpen.push_back(m_mazePieces[1][1]);
		m_mazePieces[1][1]->Traversed = true;
		m_mazePieces[0][1]->Wall = false;
	}
	m_demonstratingRandomTraversal = true;
	

	
	if (m_randomTraversalOpen.size() > 0) {
		std::vector<MazePiece*> m_tempVector{ std::begin(m_randomTraversalOpen), std::end(m_randomTraversalOpen) };
		std::random_shuffle(m_tempVector.begin(), m_tempVector.end());
		m_randomTraversalOpen.clear();
		std::copy(m_tempVector.begin(), m_tempVector.end(), std::back_inserter(m_randomTraversalOpen));

		m_randomTraversalOpen.front()->Wall = false;

		glm::vec2 pos = m_randomTraversalOpen.front()->Position.xz;
		if (pos.x > 1) {
			MazePiece* nextPiece = m_mazePieces[(int)pos.x - 1][(int)pos.y];
			if (!nextPiece->Traversed) {
				nextPiece->Traversed = true;
				if (m_mazePieces[(int)pos.x - 2][(int)pos.y]->Wall) {
					if (!(std::find(m_randomTraversalOpen.begin(), m_randomTraversalOpen.end(), nextPiece) != m_randomTraversalOpen.end())) {
						m_randomTraversalOpen.push_back(nextPiece);
					}
				}
			}
			else {
				m_randomTraversalOpen.remove(nextPiece);
			}
		}
		if (pos.x < MAZE_SIZE - 2) {
			MazePiece* nextPiece = m_mazePieces[(int)pos.x + 1][(int)pos.y];
			if (!nextPiece->Traversed) {
				nextPiece->Traversed = true;
				if (m_mazePieces[(int)pos.x + 2][(int)pos.y]->Wall) {
					if (!(std::find(m_randomTraversalOpen.begin(), m_randomTraversalOpen.end(), nextPiece) != m_randomTraversalOpen.end())) {
						m_randomTraversalOpen.push_back(nextPiece);
					}
				}
			}
			else {
				m_randomTraversalOpen.remove(nextPiece);
			}
		}
		if (pos.y > 1) {
			MazePiece* nextPiece = m_mazePieces[(int)pos.x][(int)pos.y - 1];
			if (!nextPiece->Traversed) {
				nextPiece->Traversed = true;
				if (m_mazePieces[(int)pos.x][(int)pos.y - 2]->Wall) {
					if (!(std::find(m_randomTraversalOpen.begin(), m_randomTraversalOpen.end(), nextPiece) != m_randomTraversalOpen.end())) {
						m_randomTraversalOpen.push_back(nextPiece);
					}
				}
			}
			else {
				m_randomTraversalOpen.remove(nextPiece);
			}
		}
		if (pos.y < MAZE_SIZE - 2) {
			MazePiece* nextPiece = m_mazePieces[(int)pos.x][(int)pos.y + 1];
			if (!nextPiece->Traversed) {
				nextPiece->Traversed = true;
				if (m_mazePieces[(int)pos.x][(int)pos.y + 2]->Wall) {
					if (!(std::find(m_randomTraversalOpen.begin(), m_randomTraversalOpen.end(), nextPiece) != m_randomTraversalOpen.end())) {
						m_randomTraversalOpen.push_back(nextPiece);
					}
				}
			}
			else {
				m_randomTraversalOpen.remove(nextPiece);
			}
		}
		m_randomTraversalOpen.pop_front();
	}
	else {
		m_demonstratingRandomTraversal = false;
		m_mazePieces[MAZE_SIZE - 2][MAZE_SIZE - 1]->Wall = false;
		m_mazePieces[MAZE_SIZE - 2][MAZE_SIZE - 2]->Wall = false;
		for (unsigned int x = 0; x < MAZE_SIZE; x++) {
			for (unsigned int z = 0; z < MAZE_SIZE; z++) {
				m_mazePieces[x][z]->Traversed = false;
			}
		}
	}

}