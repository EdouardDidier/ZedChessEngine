#include "SearchOld.h"


/// 
/// V1 (Random Move Generator))
/// 

SearchV1::SearchV1() {
	srand((unsigned)time(NULL));

	mBestMove = Move(0);
	mEval = 0;

	mpBoard = NULL;
}

SearchV1::~SearchV1() {

}

void SearchV1::init(Board *pBoard) {
	mpBoard = pBoard;
}

void SearchV1::searchMove(int depth) {
	vector<Move> moves = mMoveGenerator.generateLegalMove(mpBoard);

	int chosenMove = rand() % moves.size();
	int i = 0;

	for (Move move : moves) {
		if (chosenMove == i) {
			mBestMove = move;
			return;
		}

		i++;
	}
}

Move SearchV1::getBestMove() {
	return mBestMove;
}

int SearchV1::getEval() {
	return mEval;
}


/// 
/// V2 (Implenting MinMax - NegaMax)
/// 
SearchV2::SearchV2() {
	srand((unsigned)time(NULL));

	mBestMove = Move(0);
	mEval = 0;

	mpBoard = NULL;

	numNodes = 0;
}

SearchV2::~SearchV2() {

}

void SearchV2::init(Board* pBoard) {
	mpBoard = pBoard;
}

void SearchV2::searchMove(int depth) {
	numNodes = 0;

	mProfiler.startMeasure();

	vector<Move> moves = mMoveGenerator.generateLegalMove(mpBoard);
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

int SearchV2::negaMax(int depth) {
	if (depth == 0)
		return mEvaluation.evaluate(mpBoard);

	vector<Move> moves = mMoveGenerator.generateLegalMove(mpBoard);
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

Move SearchV2::getBestMove() {
	return mBestMove;
}

int SearchV2::getEval() {
	return mEval;
}


SearchV3::SearchV3() {
	srand((unsigned)time(NULL));

	mBestMove = Move(0);
	mEval = 0;

	mpBoard = NULL;

	numNodes = 0;
}

SearchV3::~SearchV3() {

}

void SearchV3::init(Board* pBoard) {
	mpBoard = pBoard;
}

void SearchV3::searchMove(int depthLeft) {
	numNodes = 0;

	mProfiler.startMeasure();

	vector<Move> moves = mMoveGenerator.generateLegalMove(mpBoard);
	int alpha = INT_MIN + 1;
	int beta = INT_MAX;

	for (Move move : moves) {
		numNodes++;
		mpBoard->makeMove(move);

		int score = -alphaBeta(-beta, -alpha, depthLeft - 1);

		mpBoard->undoMove();

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

int SearchV3::alphaBeta(int alpha, int beta, int depthLeft) {
	if (depthLeft == 0)
		return mEvaluation.evaluate(mpBoard);

	vector<Move> moves = mMoveGenerator.generateLegalMove(mpBoard);
	if (moves.empty()) {
		if (mMoveGenerator.inCheck)
			return EvaluationV3::minEval - depthLeft; //TODO: Create a const Max / Min to replace INT_MIN / INT_MAX
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

Move SearchV3::getBestMove() {
	return mBestMove;
}

int SearchV3::getEval() {
	return mEval;
}