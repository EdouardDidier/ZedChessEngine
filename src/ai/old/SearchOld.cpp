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

/// 
/// V3 (Implenting AlphaBeta)
/// 

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

/// 
/// V4 (Implenting TT and Move ordering)
/// 

SearchV4::SearchV4() {
	srand((unsigned)time(NULL));

	mBestMove = Move::invalidMove();
	mEval = 0;

	mpBoard = NULL;

	numNodes = 0;
	numCutoffs = 0;

	mMoveSequence = NULL;
}

SearchV4::~SearchV4() {

}

void SearchV4::init(Board* pBoard) {

}

void SearchV4::searchMove(Board *pBoard, int depthLeft) {
	mpBoard = new Board(*pBoard);
	mTranspositionTable.init(mpBoard);

	// Reset diagnostics datas
	numNodes = 0;
	numCutoffs = 0;

	mTranspositionTable.resetStats();
	mProfiler.startMeasure();

	// Creating new array to store move sequente (if hit by TT, deeper move are currently not updated)
	mMoveSequence = new Move[depthLeft];

	cout << endl << "[AlphaBeta] Searching at depth " << depthLeft << " ..." << endl;

	// Executing the search
	alphaBeta(INT_MIN + 1, INT_MAX, depthLeft, 0);

	// Displaying results
	cout << "Move sequence: ";
	for (int i = 0; i < depthLeft; i++)
		cout << (mMoveSequence[i].isInvalid() ? "Null" : BoardRepresentation::getMoveString(mMoveSequence[i])) << ((i != depthLeft - 1) ? " | " : "");
	cout << endl;

	cout << "Eval: " << mEval << "\t";
	cout << "| Time: " << (double)mProfiler.endMeasure() / 1000.0 << "s (Avg: " << (double)mProfiler.getAverage() / 1000.0 << "s)\t";
	cout << "| Nodes: " << numNodes << "\t| Cutoffs: " << numCutoffs << endl;

	TranspositionTable::TableStats tableStats = mTranspositionTable.stats;
	cout << "Transpositions stats:" << endl;
	cout << "| Hits: " << tableStats.numHits << endl;
	cout << "| Misses: " << tableStats.numMisses << endl;
	cout << "| Writes: " << tableStats.numWrites << endl;
	cout << "| Overwrites: " << tableStats.numOverwrites << endl << endl;

	delete[] mMoveSequence;
	delete mpBoard;
}

int SearchV4::alphaBeta(int alpha, int beta, int depthLeft, int plyCount) {
	// If repetition, return draw score (limiting to 1 reptition for simplicity) ???
	if (plyCount > 0)
		if (mpBoard->isRepetition(2))
			return 1;

	int ttEval = mTranspositionTable.getEvaluation(alpha, beta, depthLeft);
	if (ttEval != TranspositionTable::invalidEval) {
		if (plyCount == 0) {
			mBestMove = mTranspositionTable.getBestMoveOfPosition();
			mEval = ttEval;
		}

		return ttEval;
	}

	if (depthLeft == 0)
		return mEvaluation.evaluate(mpBoard);

	vector<Move> moves = mMoveGenerator.generateLegalMove(mpBoard);
	orderMove(moves);

	if (moves.empty()) {
		if (mMoveGenerator.inCheck)
			return EvaluationV4::minEval - depthLeft; // Adding a depth to prefer check mates in less moves
		else
			return -1;
	}

	TranspositionTable::TableEntryType evalType = TranspositionTable::upperBound;
	Move bestMoveInPosition = Move::invalidMove();

	for (Move move : moves) {
		numNodes++;

		mpBoard->makeMove(move);
		int score = -alphaBeta(-beta, -alpha, depthLeft - 1, plyCount + 1);
		mpBoard->undoMove();

		if (score >= beta) {
			mTranspositionTable.storeEvaluation(beta, depthLeft, TranspositionTable::lowerBound, move);
			numCutoffs++;

			return beta; // Fail hard beta-cutoff
		}

		if (score > alpha) {
			evalType = TranspositionTable::exact;
			bestMoveInPosition = move;

			alpha = score; // Alpha acts like max in MiniMax

			if (plyCount == 0) {
				mBestMove = move;
				mEval = score;
			}

			mMoveSequence[plyCount] = move;
		}
	}

	mTranspositionTable.storeEvaluation(alpha, depthLeft, evalType, bestMoveInPosition);

	return alpha;
}

Move SearchV4::getBestMove() {
	return mBestMove;
}

int SearchV4::getEval() {
	return mEval;
}

void SearchV4::orderMove(vector<Move>& moves) {
	int* moveScores = new int[(int)moves.size()];
	//Move ttMove = Move::invalidMove(); TODO: consider move of transposition table

	// Evaluating moves
	for (int i = 0; i < moves.size(); i++) {
		int score = 0;
		int pieceType = Piece::pieceType(mpBoard->getPiece(moves[i].getStartSquare()));
		int capturedType = Piece::pieceType(mpBoard->getPiece(moves[i].getTargetSquare()));
		int flag = moves[i].getFlag();

		if (capturedType != Piece::none) {
			score = EvaluationV4::capturedMultiplier * getPieceValue(capturedType) - getPieceValue(pieceType);
		}

		if (pieceType == Piece::pawn) {
			if (flag == Move::Flag::promoteToQueen)
				score += EvaluationV4::queenValue;
			else if (flag == Move::Flag::promoteToRook)
				score += EvaluationV4::rookValue;
			else if (flag == Move::Flag::promoteToBishop)
				score += EvaluationV4::bishopValue;
			else if (flag == Move::Flag::promoteToKnight)
				score += EvaluationV4::knightValue;
		}

		if (mMoveGenerator.isAttackedSquare(moves[i].getTargetSquare())) {
			score -= EvaluationV4::squareControlledPenality;
		}

		// Attributing the move score
		moveScores[i] = score;
	}

	// Sorting moves
	quickSortMoves(moves, moveScores, 0, (int)moves.size() - 1);

	delete[] moveScores;
}

void SearchV4::swapMoves(vector<Move>& moves, int* moveScores, int firstIndex, int secondIndex) {
	Move tmpMove = moves[firstIndex];
	int tmpScore = moveScores[firstIndex];

	moves[firstIndex] = moves[secondIndex];
	moveScores[firstIndex] = moveScores[secondIndex];

	moves[secondIndex] = tmpMove;
	moveScores[secondIndex] = tmpScore;
}

int SearchV4::partitionMoves(vector<Move>& moves, int* moveScores, int start, int end) {
	int pivot = moveScores[start];

	int count = 0;
	for (int i = start + 1; i <= end; i++) {
		if (moveScores[i] >= pivot)
			count++;
	}

	int pivotIndex = start + count;
	swapMoves(moves, moveScores, pivotIndex, start);

	int i = start;
	int j = end;

	while (i < pivotIndex && j > pivotIndex) {
		while (moveScores[i] >= pivot)
			i++;

		while (moveScores[j] < pivot)
			j--;

		if (i < pivotIndex && j > pivotIndex)
			swapMoves(moves, moveScores, i++, j--);
	}

	return pivotIndex;
}

void SearchV4::quickSortMoves(vector<Move>& moves, int* moveScores, int start, int end) {
	if (start >= end)
		return;

	int p = partitionMoves(moves, moveScores, start, end);

	quickSortMoves(moves, moveScores, start, p - 1);
	quickSortMoves(moves, moveScores, p + 1, end);
}

int SearchV4::getPieceValue(int pieceType) {
	switch (pieceType) {
	case Piece::queen:
		return EvaluationV4::queenValue;
	case Piece::rook:
		return EvaluationV4::rookValue;
	case Piece::bishop:
		return EvaluationV4::bishopValue;
	case Piece::knight:
		return EvaluationV4::knightValue;
	case Piece::pawn:
		return EvaluationV4::pawnValue;
	}

	return 0;
}