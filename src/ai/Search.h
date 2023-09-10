#pragma once

#include <iostream>
#include <vector>
#include <future>

#include "./src/engine/Board.h"
#include "./src/engine/MoveGenerator.h"

#include "Evaluation.h"
#include "./src/engine/TranspositionTable.h"

#include "./src/utility/Profiler.h"

using namespace std;

class Search
{
protected:
	Profiler mProfiler;

	Board* mpBoard;
	MoveGenerator mMoveGenerator;

	Move mBestMove;
	int mBestEval;

public:
	Search();
	~Search();

	virtual void searchMove(Board* pBoard, int searchTime = 1000); //TODO: move time in Player class
	virtual Move getBestMove();
	virtual int getEval();
};

class SearchV5 : public Search
{
public:
	static int getPieceValue(int pieceType);

	SearchV5();
	~SearchV5();

	void searchMove(Board *pBoard, int searchTime = 1000) override;

private:
	int alphaBeta(int alpha, int beta, int depthLeft, int plyCount);
	int quiescenceSearch(int alpha, int beta);

	void orderMove(vector<Move>& moves);
	void swapMoves(vector<Move>& moves, int* moveScores, int firstIndex, int secondIndex);
	int partitionMoves(vector<Move>& moves, int* moveScores, int start, int end);
	void quickSortMoves(vector<Move>& moves, int* moveScores, int start, int end);

	void setTimer(int time);
	void requestAbort(int time);
	void abortSearch();

private:
	EvaluationV5 mEvaluation;

	std::future<void> mTimer;
	bool mAbortSearch;
	bool mAbortTimer;

	TranspositionTable mTranspositionTable;

	Move mBestMoveThisIteration;
	int mBestEvalThisIteration;

	Move *mMoveSequence;

	// Diagnostics variables
	int numNodes;
	int numCutoffs;
	int numCaptureNodes;
};

