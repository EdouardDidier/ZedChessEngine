#pragma once

#include <iostream>
#include <list>

#include "./src/engine/Board.h" 
#include "./src/engine/MoveGenerator.h"

#include "EvaluationOld.h"

using namespace std;

class SearchV1
{
public:
	SearchV1();
	~SearchV1();

	void init(Board *pBoard);

	void searchMove(int depth);
	
	Move getBestMove();
	int getEval();

private:
	Board *mpBoard;
	MoveGenerator mMoveGenerator;

	Move mBestMove;
	int mEval;
};

class SearchV2
{
public:
	SearchV2();
	~SearchV2();

	void init(Board* pBoard);

	void searchMove(int depth);
	int negaMax(int depth);

	Move getBestMove();
	int getEval();

private:
	Profiler mProfiler;

	Board* mpBoard;
	EvaluationV2 mEvaluation;
	MoveGenerator mMoveGenerator;

	Move mBestMove;
	int mEval;

	// Diagnostics variables
	int numNodes;
};

class SearchV3
{
public:
	SearchV3();
	~SearchV3();

	void init(Board* pBoard);

	void searchMove(int depthLeft);
	int alphaBeta(int alpha, int beta, int depthLeft);

	Move getBestMove();
	int getEval();

private:
	Profiler mProfiler;

	Board* mpBoard;
	EvaluationV3 mEvaluation;
	MoveGenerator mMoveGenerator;

	Move mBestMove;
	int mEval;

	// Diagnostics variables
	int numNodes;
};
