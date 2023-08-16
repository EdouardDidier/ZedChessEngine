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

void Search::searchMove(int depthLeft) {
	numNodes = 0;

	mProfiler.startMeasure();

	list<Move> moves = mMoveGenerator.generateLegalMove(mpBoard);
	int alpha = INT_MIN + 1;
	int beta = INT_MAX;

	for (Move move : moves) {
		numNodes++;
		mpBoard->makeMove(move);

		int score = -alphaBeta(-beta, -alpha, depthLeft - 1);

		mpBoard->undoMove();
		if (score >= beta) {
			return; // Fail hard beta-cutoff
		}

		if (score > alpha) {
			alpha = score; // Alpha acts like max in MiniMax
			mBestMove = move;
			mEval = alpha;
		}
	}

	cout << "AlphaBeta (Depth: " << depthLeft << ")\t| Best move: " << BoardRepresentation::getMoveString(mBestMove) << " (Eval: " << mEval << ")\t";
	cout << "| Time: " << (double)mProfiler.endMeasure() / 1000.0 << "s (Avg: " << (double)mProfiler.getAverage() / 1000.0 << "s)\t";
	cout << "| Nodes: " << numNodes << endl;
}

int Search::alphaBeta(int alpha, int beta, int depthLeft) {
	if (depthLeft == 0)
		return mEvaluation.evaluate(mpBoard);

	list<Move> moves = mMoveGenerator.generateLegalMove(mpBoard);
	if (moves.empty()) {
		if (mMoveGenerator.inCheck)
			return INT_MIN + 1;
		else
			return 0;
	}

	for (Move move : moves) {
		numNodes++;
		mpBoard->makeMove(move);

		int score = -alphaBeta(-beta, -alpha, depthLeft - 1);

		mpBoard->undoMove();

		if (score >= beta) {
			return beta; // Fail hard beta-cutoff
		}

		if (score > alpha) {
			alpha = score; // Alpha acts like max in MiniMax
		}
	}

	return alpha;
}

Move Search::getBestMove() {
	return mBestMove;
}

int Search::getEval() {
	return mEval;
}