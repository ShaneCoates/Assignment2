#ifndef _MAZE_H_
#define	_MAZE_H_
#define MAZE_SIZE 50
#include "glm.hpp"
#include <list>
class Camera;
struct GLFWwindow;
struct MazePiece {
	glm::vec3 Position;
	bool Wall;
	bool Traversed;
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
protected:
private:
	MazePiece* m_mazePieces[MAZE_SIZE][MAZE_SIZE];

	bool m_demonstratingRandomTraversal;
	//RandomTraversalStuff
	std::list<MazePiece*> m_randomTraversalOpen;
};

#endif