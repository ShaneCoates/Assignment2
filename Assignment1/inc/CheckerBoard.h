#ifndef _CHECKER_BOARD_H_
#define _CHECKER_BOARD_H_

#include <glm.hpp>
class BoardTile;
class Camera;
class CheckerBoard {
public:
	CheckerBoard();
	~CheckerBoard();

	void Update(double _dt);
	void Draw();

	void SetCamera(Camera* _camera);
protected:
private:
	
	void Move(glm::vec2 _from, glm::vec2 _to);

	BoardTile* m_tiles[8][8];
	Camera* m_camera;
	glm::vec2 m_selectedTile;
};

#endif