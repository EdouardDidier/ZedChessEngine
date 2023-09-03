#include "./src/engine/Game.h"
#include "./src/utility/CLI.h"

int main(int argc, char** args) {
	Board* pBoard = new Board();

	// Load start position from Fen string
	pBoard->loadStartPosition();

	// If argument, enter test mode
	if (argc > 1) {
		if (strcmp(args[1], "-g") == 0 || strcmp(args[1], "-graphic") == 0) {
			Game game(pBoard);

			// Initializing the game. If failed end the program with error code.
			if (!game.init()) return 1;

			game.run();

			// Freeing memory
			game.kill();

			return 0;
		}
	}

	CLI cli = CLI(pBoard);
	
	cli.init();

	cli.run();

	cli.kill();

	delete pBoard;

	return 0;
}