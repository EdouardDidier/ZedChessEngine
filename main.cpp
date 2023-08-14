#include "Game.h"
#include "CLI.h"

int main(int argc, char** args) {
	// If argument, enter test mode
	if (argc > 1) {
		if (strcmp(args[1], "-g") == 0 || strcmp(args[1], "-graphic") == 0) {
			Game* pGame = new Game();

			// Initializing the game. If failed end the program with error code.
			if (!pGame->init()) return 1;

			pGame->run();

			// Freeing memory
			pGame->kill();
			delete pGame;

			return 0;
		}
	}

	CLI cli;
	
	cli.init();

	cli.run();

	cli.kill();

	return 0;
}