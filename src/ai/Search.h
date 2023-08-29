#pragma once

#include <iostream>
#include <vector>

#include "./src/engine/Board.h"
#include "./src/engine/MoveGenerator.h"

#include "Evaluation.h"
#include "./src/engine/TranspositionTable.h"

#include "./src/utility/Profiler.h"

using namespace std;

class SearchV5
{
public:
	SearchV5();
	~SearchV5();

	void init(Board *pBoard);

	void searchMove(int maxDepth);
	int alphaBeta(int alpha, int beta, int depthLeft, int plyCount);
	
	Move getBestMove();
	int getEval();

	static int getPieceValue(int pieceType);

private:
	void orderMove(vector<Move>& moves);
	void swapMoves(vector<Move>& moves, int* moveScores, int firstIndex, int secondIndex);
	int partitionMoves(vector<Move>& moves, int* moveScores, int start, int end);
	void quickSortMoves(vector<Move>& moves, int* moveScores, int start, int end);

private:
	Profiler mProfiler;

	Board *mpBoard;
	EvaluationV5 mEvaluation;
	MoveGenerator mMoveGenerator;

	TranspositionTable mTranspositionTable;

	Move mBestMove;
	int mBestEval;

	Move mBestMoveThisIteration;
	int mBestEvalThisIteration;

	Move *mMoveSequence;

	// Diagnostics variables
	int numNodes;
	int numCutoffs;
};

