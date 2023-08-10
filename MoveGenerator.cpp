#include "MoveGenerator.h"

MoveGenerator::MoveGenerator() {
	mpBoard = NULL;

	isWhiteToMove = true;
	friendlyColour = Piece::white;
	opponentColour = Piece::black;
	friendlyColourIndex = 0;
	opponentColourIndex = 1;
}

MoveGenerator::~MoveGenerator() {

}

void MoveGenerator::init() {
	mMoves.clear();

	isWhiteToMove = mpBoard->colourToMove == Piece::white;
	friendlyColour = mpBoard->colourToMove;
	opponentColour = mpBoard->opponentColour;
	friendlyColourIndex = mpBoard->colourToMoveIndex;
	opponentColourIndex = 1 - friendlyColourIndex;
}

list<Move> MoveGenerator::generateLegalMove(Board* pBoard) {
	mProfiler.startMeasure();

	mpBoard = pBoard;

	init();

	generateKingMoves();
	generatePawnMove();
	generateSlidingMoves();
	generateKnightMoves();

	cout << "Move gen time: " << mProfiler.endMeasure() << "ms\t(Avg: " << mProfiler.getAverage() << " ms)" << endl;

	return mMoves;
}

void MoveGenerator::generateAttackMoves() {

}

void MoveGenerator::generateKingMoves() {
	int startSquare = mpBoard->kings[friendlyColourIndex];

	for (int targetSquare : mPrecomputedMoveData.kingMoves[startSquare]) {
		if (!Piece::isColour(mpBoard->getPiece(targetSquare), friendlyColour))
			mMoves.push_back(Move(startSquare, targetSquare));

		// Handle castle situations
		if (Piece::pieceType(mpBoard->getPiece(targetSquare)) == Piece::none) {
			// King side (5 = f1 / 61 = f8)
			if ((targetSquare == 5 || targetSquare == 61) && this->hasKingSideCastleRight()) {
				int castleSquare = targetSquare + 1;

				if (Piece::pieceType(mpBoard->getPiece(castleSquare)) == Piece::none) {
					mMoves.push_back(Move(startSquare, castleSquare, Move::Flag::castling));
				}

			}

			// Queen side (3 = d1 / d59 = 8)
			if ((targetSquare == 3 || targetSquare == 59) && this->hasQueenSideCastleRight()) {
				int castleSquare = targetSquare - 1;

				if ((Piece::pieceType(mpBoard->getPiece(castleSquare)) == Piece::none)
					&& (Piece::pieceType(mpBoard->getPiece(castleSquare - 1)) == Piece::none)) {
					mMoves.push_back(Move(startSquare, castleSquare, Move::Flag::castling));
				}
			}
		}
	}
}

void MoveGenerator::generatePawnMove() {
	int offset = (friendlyColour == Piece::white) ? 8 : -8;
	int startRank = isWhiteToMove ? 1 : 6;
	int finalRankBeforePromotion = isWhiteToMove ? 6 : 1;

	// Check and store if ep is possible
	int epFile = ((int)(mpBoard->currentGameState >> 4) & 0b111);
	int epSquare = -1;
	if (epFile != -1) {
		epSquare = 8 * ((mpBoard->whiteToMove) ? 5 : 2) + epFile;
	}
	
	for (int i = 0 ; i < mpBoard->pawns[friendlyColourIndex]->count(); i++) {
		int startSquare = (*mpBoard->pawns[friendlyColourIndex])[i];

		int squareForward = startSquare + offset;
		int pawnRank = startSquare / 8;
		bool oneRankToPromotion = pawnRank == finalRankBeforePromotion;

		if (mpBoard->getPiece(squareForward) == Piece::none) {
			if (oneRankToPromotion) {
				makePromotionMoves(startSquare, squareForward);
			}
			else {
				mMoves.push_back(Move(startSquare, squareForward)); 
					
				if ((pawnRank) == startRank) {
					int squareTwoForward = squareForward + offset;

					if (mpBoard->getPiece(squareTwoForward) == Piece::none) {
						mMoves.push_back(Move(startSquare, squareTwoForward, Move::Flag::pawnTwoForward));
					}
				}
			}
		}

		for (int j = 0; j < 2; j++) {
			if (mPrecomputedMoveData.numSquaresToEdge[startSquare][mPrecomputedMoveData.pawnAttackDirections[friendlyColourIndex][j]] > 0) {
				int pawnCaptureDir = mPrecomputedMoveData.directionOffsets[mPrecomputedMoveData.pawnAttackDirections[friendlyColourIndex][j]];
				int targetSquare = startSquare + pawnCaptureDir;
				int targetPiece = mpBoard->getPiece(targetSquare);


				// Regular capture
				if (Piece::isColour(targetPiece, opponentColour)) {
					if (oneRankToPromotion)
						makePromotionMoves(startSquare, targetSquare);
					else
						mMoves.push_back(Move(startSquare, targetSquare));
				}

				// Capture en-passant
				if (targetSquare == epSquare) {
					mMoves.push_back(Move(startSquare, targetSquare, Move::Flag::enPassantCapture));
				}
			}
		}
	}
}

void MoveGenerator::generateSlidingMoves() {
	for (int i = 0; i < mpBoard->queens[friendlyColourIndex]->count(); i++)
		generateSlidingPieceMoves((*mpBoard->queens[friendlyColourIndex])[i], 0, 8);


	for (int i = 0; i < mpBoard->rooks[friendlyColourIndex]->count(); i++)
		generateSlidingPieceMoves((*mpBoard->rooks[friendlyColourIndex])[i], 0, 4);


	for (int i = 0; i < mpBoard->bishops[friendlyColourIndex]->count(); i++)
		generateSlidingPieceMoves((*mpBoard->bishops[friendlyColourIndex])[i], 4, 8);
}

void MoveGenerator::generateSlidingPieceMoves(int startSquare, int startIndex, int endIndex) {
	for (int dirIndex = startIndex; dirIndex < endIndex; dirIndex++) {
		int offsetDirection = mPrecomputedMoveData.directionOffsets[dirIndex];

		for (int i = 0; i < mPrecomputedMoveData.numSquaresToEdge[startSquare][dirIndex]; i++) {
			int targetSquare = startSquare + offsetDirection * (i + 1);
			int targetPiece = mpBoard->getPiece(targetSquare);

			if (Piece::isColour(targetPiece, friendlyColour))
				break;

			mMoves.push_back(Move(startSquare, targetSquare));

			if (Piece::isColour(targetPiece, opponentColour))
				break;	
		}
	}
}

void MoveGenerator::generateKnightMoves() {
	for (int i = 0; i < mpBoard->knights[friendlyColourIndex]->count(); i++) {
		int startSquare = (*mpBoard->knights[friendlyColourIndex])[i];

		for (int targetSquare : mPrecomputedMoveData.knightMoves[startSquare]) {
			if (!Piece::isColour(mpBoard->getPiece(targetSquare), friendlyColour))
				mMoves.push_back(Move(startSquare, targetSquare));
		}
	}
}


void MoveGenerator::makePromotionMoves(int fromSquare, int toSquare) {
	mMoves.push_back(Move(fromSquare, toSquare, Move::Flag::promoteToQueen));
	mMoves.push_back(Move(fromSquare, toSquare, Move::Flag::promoteToRook));
	mMoves.push_back(Move(fromSquare, toSquare, Move::Flag::promoteToBishop));
	mMoves.push_back(Move(fromSquare, toSquare, Move::Flag::promoteToKnight));
}

bool MoveGenerator::hasKingSideCastleRight() {
	int mask = (mpBoard->whiteToMove) ? 1 : 4;
	return (mpBoard->currentGameState & mask) != 0;
}

bool MoveGenerator::hasQueenSideCastleRight() {
	int mask = (mpBoard->whiteToMove) ? 2 : 8;
	return (mpBoard->currentGameState & mask) != 0;
}