#include "Perft.h"

Perft::Perft() {

}

Perft::~Perft() {

}

void Perft::runTestDivided(Board *pBoard, int depth) {
	Uint64 total = 0;
	vector<Move> moves = mMoveGenerator.generateLegalMove(pBoard);

	for (Move move : moves) {
		pBoard->makeMove(move);

		Uint64 n = 0;

		if (depth > 1)
			n += perftQuick(pBoard, depth - 1);
		else
			n = 1;
		
		total += n;

		pBoard->undoMove();

		cout << BoardRepresentation::getMoveString(move) << ":\t" << n << endl;
	}

	cout << endl << "Nodes searched: " << total << endl << endl;
}

void Perft::runTestQuick(Board* pBoard, int maxDepth) {
	for (int i = 1; i < maxDepth + 1; i++) {
		mProfiler.startMeasure();
		Uint64 n = perftQuick(pBoard, i);
		Uint64 elapsed = mProfiler.endMeasure();

		cout << "[PERFT] Depth: " << i << "\t| Time: " << (double)elapsed / 1000.0 << "s" << "\t| # nodes: " << n << endl;
	}
}

void Perft::runTestFull(Board* pBoard, int maxDepth) {
	for (int i = 1; i < maxDepth + 1; i++) {
		mProfiler.startMeasure();
		perftFull(pBoard, i);
		Uint64 elapsed = mProfiler.endMeasure();

		cout << "[PERFT] Depth: " << i << "\t| Time: " << (double)elapsed / 1000.0 << "s" << "\t| # nodes: " << nodes << endl;
		cout << "\t| Captures: " << captures << "\t| Ep: " << ep << endl;
		cout << "\t| Castles: " << castles << "\t| Promotions: " << promotions << endl << endl;

		nodes = 0;
		captures = 0;
		ep = 0;
		castles = 0;
		promotions = 0;
	}
}

Uint64 Perft::perftQuick(Board* pBoard, int depth) {
	vector<Move> moves = mMoveGenerator.generateLegalMove(pBoard);
	Uint64 n = 0;

	if (depth == 1)
		return (Uint64)moves.size();

	for (Move move : moves) {
		pBoard->makeMove(move);
		n += perftQuick(pBoard, depth - 1);
		pBoard->undoMove();
	}

	return n;
}

void Perft::perftFull(Board* pBoard, int depth) {
	vector<Move> moves = mMoveGenerator.generateLegalMove(pBoard);

	if (depth == 0)
		return;

	for (Move move : moves) {
		if (depth == 1) {
			nodes++;

			if (pBoard->getPiece(move.getTargetSquare()) != Piece::none)
				captures++;

			if (move.isEnPassant())
				ep++;

			if (move.isCastle())
				castles++;

			if (move.isPromotion())
				promotions++;
		}

		pBoard->makeMove(move);
		perftFull(pBoard, depth - 1);
		pBoard->undoMove();
	}
}