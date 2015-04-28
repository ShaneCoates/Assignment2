/*
	Author: Shane Coates
	Description: Main game loop
*/

#include "Game.h"
void main() {
	Game* theGame = new Game();
	theGame->Run();
	delete theGame;
}