#pragma once

#include <iostream>
#include <string>
#include <list>

#include <SDL.h>

#include "Game.h"

#include "Board.h"
#include "BoardRepresentation.h"

#include "Move.h"
#include "Piece.h"

#include "Perft.h"
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

