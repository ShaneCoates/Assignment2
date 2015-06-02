#ifndef _MAZE_H_
#define	_MAZE_H_
#define MAZE_SIZE 25
#include "glm.hpp"
#include <list>
#include <vector>
class Camera;
struct GLFWwindow;
struct MazePiece {
	glm::vec3 Position;
	bool Wall;
	bool Traversed;
	bool InOpenList;
};
class Maze {
public:
	Maze();
	~Maze();

	void Update(double _dt);
	void Draw(Camera* _camera);

	void ResetMaze();
	void Stop();

	void RandomTraversal();
	void DemonstrateRandomTraversal();

	bool m_wireFrame;

protected:
private:

	MazePiece* GetRandomNeighbor(MazePiece* _start);

	MazePiece* North(glm::vec2 _pos);
	MazePiece* South(glm::vec2 _pos);
	MazePiece* East(glm::vec2 _pos);
	MazePiece* West(glm::vec2 _pos);


	MazePiece* m_mazePieces[MAZE_SIZE][MAZE_SIZE];

	bool m_demonstratingRandomTraversal;
	//RandomTraversal stuff
	std::list<MazePiece*> m_randomTraversalOpen;
};

#endif