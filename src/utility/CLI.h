#pragma once

#include <iostream>
#include <string>
#include <list>

#include <SDL.h>

#include "./src/engine/Game.h"

#include "./src/engine/Board.h"
#include "./src/utility/BoardRepresentation.h"

#include "./src/engine/Move.h"
#include "./src/engine/Piece.h"

#include "./src/testing/Perft.h"
#include "Profiler.h"

using namespace std;

class CLI
{
public:
	CLI();
	~CLI();

	bool init();
	void kill();

	void run();

	void loadFen(string str);

	void makeMove(string str);
	void redoMove();
	void undoMove();

	void displayLegalMove();
	void displayBoard();
	char getPieceChar(int index);

	bool startGraphicUI();

private:
	Board *mpBoard;
	MoveGenerator mMoveGenerator;

	Profiler mProfiler;
};

