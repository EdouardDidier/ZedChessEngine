#include "Search.h"

SearchV5::SearchV5() {
	srand((unsigned)time(NULL));

	mBestMove = Move::invalidMove();
	mBestEval = 0;

	mBestMoveThisIteration = mBestMove;
	mBestEvalThisIteration = mBestEval;

	mpBoard = NULL;
	mAbortSearch = false;
	mAbortTimer = false;

	numNodes = 0;
	numCutoffs = 0;
	numCaptureNodes = 0;

	mMoveSequence = NULL;
}

SearchV5::~SearchV5() {

}

void SearchV5::searchMove(Board *pBoard, int searchTime, int maxDepth) {
	mProfiler.startMeasure();

	setTimer(searchTime);

	mpBoard = new Board(*pBoard);
	mTranspositionTable.init(mpBoard);

	// Reset diagnostics datas
	numNodes = 0;
	numCutoffs = 0;
	numCaptureNodes = 0;

	mTranspositionTable.resetStats();

	// Creating new array to store move sequente (if hit by TT, deeper move are currently not updated)
	mMoveSequence = new Move[maxDepth + 1];

	cout << endl << "[AlphaBeta] " << (mpBoard->whiteToMove ? "White" : "Black") << " - Searching for " << (searchTime / 1000) << "s ..." << endl;

	// Executing iterative deepening search
	int currentSearchDepth = 0;
	for (int depth = 1; depth <= maxDepth; depth++) {
	
		alphaBeta(INT_MIN + 1, INT_MAX, depth, 0);

		currentSearchDepth = depth;

		mBestMove = mBestMoveThisIteration;
		mBestEval = mBestEvalThisIteration;

		if (mAbortSearch) {
			cout << "Search aborted" << endl;
			break;
		}

		// Displaying moves at each iteration
		cout << "(Depth: " << depth  << ") Eval: " << mBestEval << "\t| ";
		for (int i = 0; i < depth; i++)
			cout << (mMoveSequence[i].isInvalid() ? "Null" : BoardRepresentation::getMoveString(mMoveSequence[i])) << ((i != depth - 1) ? " - " : "");
		cout << endl;

		// If mate score found, quit the search, all later mate will take more moves
		if (mBestEval >= EvaluationV5::maxEval)
			break;
	}

	// Displayinng results
	cout << endl << "Generals stats:" << endl;
	cout << "| Time: " << (double)mProfiler.endMeasure() / 1000.0 << "s (Avg: " << (double)mProfiler.getAverage() / 1000.0 << "s)\t";
	cout << "| Depth: " << currentSearchDepth << "\t| Nodes: " << numNodes << "\t| Cutoffs: " << numCutoffs << "\t| Captured nodes: " << numCaptureNodes << endl << endl;

	TranspositionTable::TableStats tableStats = mTranspositionTable.stats;
	cout << "Transpositions stats:" << endl;
	cout << "| Hits: " << tableStats.numHits << endl;
	cout << "| Misses: " << tableStats.numMisses << endl;
	cout << "| Writes: " << tableStats.numWrites << endl;
	cout << "| Overwrites: " << tableStats.numOverwrites << endl << endl;

	delete[] mMoveSequence;
	delete mpBoard;
}

int SearchV5::alphaBeta(int alpha, int beta, int depthLeft, int plyCount) {
	if (mAbortSearch)
		return 0;

	// If repetition, return draw score (limiting to 1 reptition for simplicity)
	if (plyCount > 0)
		if (mpBoard->isRepetition(2))
			return 1; //TODO: replace with draw score with ply count (favorising later draw)

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
		return quiescenceSearch(alpha, beta);

	vector<Move> moves = mMoveGenerator.generateLegalMove(mpBoard);
	orderMove(moves);

	if (moves.empty()) {
		if (mMoveGenerator.inCheck)
			return EvaluationV5::minEval - depthLeft; // Adding a depth to prefer check mates in less moves //TODO: replace with a mate score and probably change with a +plycount
		else
			return -1; //TODO: replace with a draw score
	}

	TranspositionTable::TableEntryType evalType = TranspositionTable::upperBound;
	Move bestMoveInPosition = Move::invalidMove();

	for (Move move : moves) {
		numNodes++;

		mpBoard->makeMove(move);
		int score = -alphaBeta(-beta, -alpha, depthLeft - 1, plyCount + 1);
		mpBoard->undoMove();
	
		if (mAbortSearch)
			return 0;

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

int SearchV5::quiescenceSearch(int alpha, int beta) {
	// Making a first eval to avoid bad capture, accepting potential non-capture move
	int score = mEvaluation.evaluate(mpBoard);

	if (score >= beta)
		return beta;

	if (score > alpha)
		alpha = score;

	vector<Move> moves = mMoveGenerator.generateLegalMove(mpBoard, true);
	orderMove(moves);

	for (Move move : moves) {
		mpBoard->makeMove(move);
		score = -quiescenceSearch(-beta, -alpha);
		mpBoard->undoMove();

		numNodes++;
		numCaptureNodes++;

		if (score >= beta) {
			numCutoffs++;
			return beta;
		}

		if (score > alpha) {
			alpha = score;
		}
	}

	return alpha;
}

Move SearchV5::getBestMove() {
	return mBestMove;
}

int SearchV5::getEval() {
	return mBestEval;
}

void SearchV5::setTimer(int time) {
	abortSearch();

	mAbortSearch = false;
	mTimer = std::async(std::launch::async, &SearchV5::requestAbort, this, time);
}

void SearchV5::requestAbort(int time) {
	const int step = 50;
	int count = 0;
	
	while (count < time && !mAbortTimer) {
		SDL_Delay(step);
		count += step;
	}
	
	mAbortSearch = true;
}

void SearchV5::abortSearch() {
	// If another timer is already running, abort it before starting a new one
	if (mTimer.valid()) {
		if (mTimer.wait_for(std::chrono::seconds(0)) == std::future_status::timeout) {
			mAbortTimer = true;
			mTimer.wait();
			mAbortTimer = false;
		}
	}
}

void SearchV5::orderMove(vector<Move>& moves) {
	int *moveScores = new int[(int)moves.size()];
	Move ttMove = mTranspositionTable.getBestMoveOfPosition();

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