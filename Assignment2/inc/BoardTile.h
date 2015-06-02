#ifndef _BOARD_TILE_H_
#define	_BOARD_TILE_H_
#include <glm.hpp>
class Camera;
enum TileType {
	eWhite = 0,
	eEmpty = 1,
	eBlackPiece = 2,
	eRedPiece = 3
};
class BoardTile {
public:
	BoardTile();
	BoardTile(glm::vec2 _position);
	~BoardTile();

	void Update(double _dt);
	void Draw(Camera* _camera);

	void Hover();
	void Press(BoardTile* _tiles[8][8]);
	void Highlight();

	void Deselect();

	bool IsOpen() { return m_open; }
	bool IsSelected() { return m_selected; }

	TileType m_type;
	glm::vec2 m_position;
	bool m_king;
protected:
private:

	glm::vec4 m_colour;
	bool m_selected;
	bool m_open;
};

#endif