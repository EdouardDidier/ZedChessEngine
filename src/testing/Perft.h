#pragma once

#include <iostream>
#include <list>

#include <SDL.h>

#include "./src/engine/Board.h"
#include "./src/engine/Move.h"
#include "./src/engine/MoveGenerator.h"

#include "./src/utility/BoardRepresentation.h"
#include "./src/utility/Profiler.h"

class Perft
{
public:
	Perft();
	~Perft();

	void runTestDivided(Board* pBoard, int depth = 1);
	void runTestQuick(Board* pBoard, int maxDepth = 1);
	void runTestFull(Board* pBoard, int maxDepth = 1);

	Uint64 perftQuick(Board* pBoard, int depth);
	void perftFull(Board* pBoard, int depth);

	Uint64 nodes = 0;
	Uint64 captures = 0;
	Uint64 ep = 0;
	Uint64 castles = 0;
	Uint64 promotions = 0;

private:
	MoveGenerator mMoveGenerator;

	Profiler mProfiler;
};

