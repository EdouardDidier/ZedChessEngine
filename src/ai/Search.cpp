#include "Search.h"

SearchV5::SearchV5() {
	srand((unsigned)time(NULL));

	mBestMove = Move::invalidMove();
	mBestEval = 0;

	mBestMoveThisIteration = mBestMove;
	mBestEvalThisIteration = mBestEval;

	mpBoard = NULL;

	numNodes = 0;
	numCutoffs = 0;

	mMoveSequence = NULL;
}

SearchV5::~SearchV5() {

}

void SearchV5::init(Board *pBoard) {
	mpBoard = pBoard;
	mTranspositionTable.init(pBoard);
}

void SearchV5::searchMove(int maxDepth) {
	// Reset diagnostics datas
	numNodes = 0;
	numCutoffs = 0;

	mTranspositionTable.resetStats();
	mProfiler.startMeasure();

	// Creating new array to store move sequente (if hit by TT, deeper move are currently not updated)
	mMoveSequence = new Move[maxDepth + 1];

	cout << endl << "[AlphaBeta] " << (mpBoard->whiteToMove ? "White" : "Black") << " - Searching at depth " << maxDepth << " ..." << endl;

	// Executing iterative deepening search
	int currentSearchDepth = 0;
	for (int depth = 1; depth <= maxDepth; depth++) {
		currentSearchDepth = depth;
		alphaBeta(INT_MIN + 1, INT_MAX, depth, 0);

		mBestMove = mBestMoveThisIteration;
		mBestEval = mBestEvalThisIteration;

		// Displaying moves at each iteration
		cout << "(Depth: " << depth  << ") Eval: " << mBestEvalThisIteration << "\t| ";
		for (int i = 0; i < depth; i++)
			cout << (mMoveSequence[i].isInvalid() ? "Null" : BoardRepresentation::getMoveString(mMoveSequence[i])) << ((i != depth - 1) ? " - " : "");
		cout << endl;
	}

	// Displayinng results
	cout << endl << "Generals stats:" << endl;
	cout << "| Time: " << (double)mProfiler.endMeasure() / 1000.0 << "s (Avg: " << (double)mProfiler.getAverage() / 1000.0 << "s)\t";
	cout << "| Nodes: " << numNodes << "\t| Cutoffs: " << numCutoffs << endl << endl;

	TranspositionTable::TableStats tableStats = mTranspositionTable.stats;
	cout << "Transpositions stats:" << endl;
	cout << "| Hits: " << tableStats.numHits << endl;
	cout << "| Misses: " << tableStats.numMisses << endl;
	cout << "| Writes: " << tableStats.numWrites << endl;
	cout << "| Overwrites: " << tableStats.numOverwrites << endl << endl;

	delete[] mMoveSequence;
}

int SearchV5::alphaBeta(int alpha, int beta, int depthLeft, int plyCount) {
	// If repetition, return draw score (limiting to 1 reptition for simplicity) TODO: Check if works correctly with transposition table
	if (plyCount > 0)
		if (mpBoard->isRepetition(2))
			return 1;

	int ttEval = mTranspositionTable.getEvaluation(alpha, beta, depthLeft);
	if (ttEval != TranspositionTable::invalidEval) {
		if (plyCount == 0) {
			mBestMoveThisIteration = mTranspositionTable.getBestMoveOfPosition();
			mBestEvalThisIteration = ttEval;
		}

		mMoveSequence[plyCount] = mTranspositionTable.getBestMoveOfPosition();

		return ttEval;
	}
	
	if (depthLeft == 0)
		return mEvaluation.evaluate(mpBoard);

	vector<Move> moves = mMoveGenerator.generateLegalMove(mpBoard);
	orderMove(moves);

	if (moves.empty()) {
		if (mMoveGenerator.inCheck)
			return EvaluationV5::minEval - depthLeft; // Adding a depth to prefer check mates in less moves
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
				mBestMoveThisIteration = move;
				mBestEvalThisIteration = score;
			}

			mMoveSequence[plyCount] = move;
		}
	}

	mTranspositionTable.storeEvaluation(alpha, depthLeft, evalType, bestMoveInPosition);

	return alpha;
}

Move SearchV5::getBestMove() {
	return mBestMove;
}

int SearchV5::getEval() {
	return mBestEval;
}

void SearchV5::orderMove(vector<Move>& moves) {
	int *moveScores = new int[(int)moves.size()];
	Move ttMove = mTranspositionTable.getBestMoveOfPosition(); //TODO: consider move of transposition table

	// Evaluating moves
	for (int i = 0; i < moves.size(); i++) {
		int score = 0;
		int pieceType = Piece::pieceType(mpBoard->getPiece(moves[i].getStartSquare()));
		int capturedType = Piece::pieceType(mpBoard->getPiece(moves[i].getTargetSquare()));
		int flag = moves[i].getFlag();

		if (capturedType != Piece::none) {
			score = EvaluationV5::capturedMultiplier * getPieceValue(capturedType) - getPieceValue(pieceType);
		}

		if (pieceType == Piece::pawn) {
			if (flag == Move::Flag::promoteToQueen)
				score += EvaluationV5::queenValue;
			else if (flag == Move::Flag::promoteToRook)
				score += EvaluationV5::rookValue;
			else if (flag == Move::Flag::promoteToBishop)
				score += EvaluationV5::bishopValue;
			else if (flag == Move::Flag::promoteToKnight)
				score += EvaluationV5::knightValue;
		}

		if (mMoveGenerator.isAttackedSquare(moves[i].getTargetSquare())) {
			score -= EvaluationV5::squareControlledPenality;
		}

		if (Move::isSameMove(moves[i], ttMove)) {
			score += 10000;
		}

		// Attributing the move score
		moveScores[i] = score;
	}

	// Sorting moves
	quickSortMoves(moves, moveScores, 0, (int)moves.size() - 1);

	delete[] moveScores;
}

void SearchV5::swapMoves(vector<Move>& moves, int* moveScores, int firstIndex, int secondIndex) {
	Move tmpMove = moves[firstIndex];
	int tmpScore = moveScores[firstIndex];

	moves[firstIndex] = moves[secondIndex];
	moveScores[firstIndex] = moveScores[secondIndex];

	moves[secondIndex] = tmpMove;
	moveScores[secondIndex] = tmpScore;
}

int SearchV5::partitionMoves(vector<Move>& moves, int* moveScores, int start, int end) {
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

		if ( i < pivotIndex && j > pivotIndex)
			swapMoves(moves, moveScores, i++, j--);
	}

	return pivotIndex;
}

void SearchV5::quickSortMoves(vector<Move>& moves, int *moveScores, int start, int end) {
	if (start >= end)
		return;

	int p = partitionMoves(moves, moveScores, start, end);

	quickSortMoves(moves, moveScores, start, p - 1);
	quickSortMoves(moves, moveScores, p + 1, end);
}

int SearchV5::getPieceValue(int pieceType) {
	switch (pieceType) {
	case Piece::queen:
		return EvaluationV5::queenValue;
	case Piece::rook:
		return EvaluationV5::rookValue;
	case Piece::bishop:
		return EvaluationV5::bishopValue;
	case Piece::knight:
		return EvaluationV5::knightValue;
	case Piece::pawn:
		return EvaluationV5::pawnValue;
	}

	return 0;
}