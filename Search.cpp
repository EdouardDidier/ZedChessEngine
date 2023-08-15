#include "Search.h"

Search::Search() {
	srand((unsigned)time(NULL));

	mBestMove = Move(0);
	mEval = 0;

	mpBoard = NULL;

	numNodes = 0;
}

Search::~Search() {

}

void Search::init(Board *pBoard) {
	mpBoard = pBoard;
}

void Search::searchMove(int depth) {
	numNodes = 0;

	mProfiler.startMeasure();

	list<Move> moves = mMoveGenerator.generateLegalMove(mpBoard);
	int max = INT_MIN;
	
	for (Move move : moves) {
		numNodes++;

		mpBoard->makeMove(move);

		int score = -negaMax(depth - 1);
		if (score > max) {
			max = score;
			mBestMove = move;
			mEval = max;
		}

		mpBoard->undoMove();
	}

	cout << "MinMax (Depth: " << depth << ")\t| Best move: " << BoardRepresentation::getMoveString(mBestMove) << " (Eval: " << mEval << ")\t";
	cout << "| Time: " << (double)mProfiler.endMeasure() / 1000.0 << "s (Avg: " << (double)mProfiler.getAverage() / 1000.0 << "s)\t";
	cout << "| Nodes: " << numNodes << endl;
}

int Search::negaMax(int depth) {
	if (depth == 0)
		return mEvaluation.evaluate(mpBoard);

	list<Move> moves = mMoveGenerator.generateLegalMove(mpBoard);
	if (moves.empty()) {
		if (mMoveGenerator.inCheck)
			return INT_MIN + 1;
		else
			return 0;
	}

	int max = INT_MIN;

	for (Move move : moves) {
		numNodes++;

		mpBoard->makeMove(move);

		int score = -negaMax(depth - 1);
		if (score > max) {
			max = score;
		}

		mpBoard->undoMove();
	}

	return max;
}

Move Search::getBestMove() {
	return mBestMove;
}

int Search::getEval() {
	return mEval;
}