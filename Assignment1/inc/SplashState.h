/*
	Author: Shane Coates
	Description: Splash State
*/

#ifndef _SPLASH_STATE_H_
#define	_SPLASH_STATE_H_

#include "GameStateManager.h"
struct GLFWwindow;
class SplashState : public IGameState
{
public:
	//Constructor inherits from IGameState and calls the Init function
	SplashState(GLFWwindow* _window, GameStateManager* _gameStateManager) : IGameState()
	{
		Init(_window, _gameStateManager);
	}
	//Destructor
	~SplashState();

	//Initialise the gamestate
	void Init(GLFWwindow* _window, GameStateManager* _gameStateManager);

	//Update/Draw functions
	void Update(double _dt);
	void Draw();

private:
	GLFWwindow* m_window;
	GameStateManager* m_gameStateManager;
	
	double m_timeLeft;
};

#endif