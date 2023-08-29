#pragma once

#include <iostream>
#include <list>

#include "./src/engine/Board.h" 
#include "./src/engine/MoveGenerator.h"
#include "./src/engine/TranspositionTable.h"

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

class SearchV4
{
public:
	SearchV4();
	~SearchV4();

	void init(Board* pBoard);

	void searchMove(int depthLeft);
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

	Board* mpBoard;
	EvaluationV4 mEvaluation;
	MoveGenerator mMoveGenerator;

	TranspositionTable mTranspositionTable;

	Move mBestMove;
	int mEval;

	Move* mMoveSequence;

	// Diagnostics variables
	int numNodes;
	int numCutoffs;
};