/*
Author: Shane Coates
Description: Game State Manager class.
Holds a list of states and updates/draws them
*/

#ifndef _GAMESTATE_MANAGER_H_
#define	_GAMESTATE_MANAGER_H_

#include <map>
#include <list>
#include <string>

//IGameState class - to be inherited by actual states
class IGameState;

class GameStateManager
{
public:
	//Constructor
	GameStateManager();
	//Destructor
	~GameStateManager();

	//Update and Draw functions
	void Update(double _dt);
	void Draw();

	//Creates a new state and adds it to the map of states
	void RegisterState(std::string _name, IGameState* _state);

	//Pushes a state onto the stack after looking for it's name in the map
	bool Push(std::string _name);

	//Remove the top state from the stack
	void Pop();

private:
	std::map<std::string, IGameState*> m_gameStates;
	std::list<IGameState*> m_stack;

	unsigned int m_toPop;
};

class IGameState
{
public:

	~IGameState() {};

	//Update and Draw functions to be overridden
	virtual void Update(double dt) {};
	virtual void Draw() {};

	//Check to see if it blocks the update/draw functions of other states on the stack
	bool isUpdateBlocking() { return m_updateBlocking; }
	bool isDrawBlocking()	{ return m_drawBlocking; }

	//Sets blocking status'
	void SetUpdateBlocking(bool _block) { m_updateBlocking = _block; }
	void SetDrawBlocking(bool _block) { m_drawBlocking = _block; }

protected:
private:
	bool m_updateBlocking = true;
	bool m_drawBlocking = true;
};

#endif