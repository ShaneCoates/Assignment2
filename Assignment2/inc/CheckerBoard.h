#ifndef _CHECKER_BOARD_H_
#define _CHECKER_BOARD_H_

#include <glm.hpp>
#include <vector>
class BoardTile;
class Camera;
class NetworkManager;

class CheckerBoard {
public:

	struct PossibleMove {
		glm::vec2 From;
		glm::vec2 To;
	};

	CheckerBoard();
	~CheckerBoard();

	void Update(double _dt);
	void Draw();

	void SetCamera(Camera* _camera) { m_camera = _camera; }

	void CheckForMoves();

	void SetNetworkManager(NetworkManager* _manager) { m_networkManager = _manager; m_networkManagerInitialised = true; }

	bool TakeAvailable();

	bool m_controllingBlack;

protected:
private:

	void CheckGameOver();

	void GameOver();

	void CheckForTakeAvailable();

	void DeselectAll();

	bool m_blackTurn;
	BoardTile* GetMouseOver();

	void Move(glm::vec2 _from, glm::vec2 _to);

	BoardTile* m_tiles[8][8];
	Camera* m_camera;
	glm::vec2 m_selectedTile;
	NetworkManager* m_networkManager;
	bool m_networkManagerInitialised;
	bool m_mouseDown;
	
	std::vector<PossibleMove> m_availableTakes;
};

#endif