/* 
	Author: Shane Coates
	Description: Test state for shadows
*/

#ifndef _PROCEDURAL_GENERATION_H_
#define	_PROCEDURAL_GENERATION_H_
#include "GameStateManager.h"
#include "glm.hpp"
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#define IM_ARRAYSIZE(_ARR)          ((int)(sizeof(_ARR)/sizeof(*_ARR)))

struct GLFWwindow;
class FlyCamera;
class ObjectFBX;
class PerlinParticleEmitter;
class Skybox;
class Terrain;
class ParticleEmitter;
struct Vertex{ 
	glm::vec4 Position;
	glm::vec2 UV;
};
class Procedural : public IGameState
{
public:
	//Constructor inherits from IGameState and calls the Init function
	Procedural(GLFWwindow* _window, GameStateManager* _gameStateManager) : IGameState()
	{
		Init(_window, _gameStateManager);
	}
	//Destructor
	~Procedural();

	//Initialise the gamestate
	void Init(GLFWwindow* _window, GameStateManager* _gameStateManager);

	//Update/Draw functions
	void Update(double _dt);
	void Draw();

protected:
private:

	GLFWwindow* m_window;
	GameStateManager* m_gameStateManager;
	FlyCamera* m_camera;
	ObjectFBX* m_testObject;
	Skybox* m_skybox;
	Terrain* m_terrain;

	unsigned int m_seedCount;
	ImVector<int> m_seeds;

	ParticleEmitter* m_emitter;
};

#endif