#pragma once

#include <iostream>
#include <list>

#include "Board.h"
#include "MoveGenerator.h"

#include "Evaluation.h"

#include "Profiler.h"

using namespace std;

class Search
{
public:
	Search();
	~Search();

	void init(Board *pBoard);

	void searchMove(int depthLeft);
	int alphaBeta(int alpha, int beta, int depthLeft);
	
	Move getBestMove();
	int getEval();

private:
	Profiler mProfiler;

	Board *mpBoard;
	Evaluation mEvaluation;
	MoveGenerator mMoveGenerator;

	Move mBestMove;
	int mEval;

	// Diagnostics variables
	int numNodes;
};

