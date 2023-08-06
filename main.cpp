#include "Game.h"

int main(int argc, char** args) {
	Game *pGame = new Game();

	// Initializing the game. If failed end the program with error code.
	if (!pGame->init()) return 1;

	pGame->run();

	// Freeing memory
	pGame->kill();
	delete pGame;

	// Ending program
	return 0;
}