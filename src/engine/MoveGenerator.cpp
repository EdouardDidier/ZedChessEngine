#include "MoveGenerator.h"

MoveGenerator::MoveGenerator() {
	mpBoard = NULL;

	isWhiteToMove = true;
	friendlyColour = Piece::white;
	opponentColour = Piece::black;
	friendlyColourIndex = 0;
	opponentColourIndex = 1;

	mCapturedOnly = false;

	resetAttackData();
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

	resetAttackData();
}

void MoveGenerator::resetAttackData() {
	inCheck = false;
	inDoubleCheck = false;

	for (int i = 0; i < 64; i++) {
		attackedSquares[i] = false;
		checkRaySquares[i] = false;
		pinRaySquares[i] = false;
	}
}

vector<Move> MoveGenerator::generateLegalMove(Board* pBoard, bool capturedOnly) {
	//mProfiler.startMeasure();

	mpBoard = pBoard;
	mCapturedOnly = capturedOnly;

	init();

	if (pBoard->isRepetition())
		return mMoves;

	generateAttackData();

	generateKingMoves();

	// In case of double checks, only kings moves are allowed (other move would remove only one of the checks)
	if (inDoubleCheck)
		return mMoves;

	generatePawnMove();
	generateSlidingMoves();
	generateKnightMoves();

	//cout << "Move gen time: " << mProfiler.endMeasure() << "ms\t(Avg: " << mProfiler.getAverage() << " ms)" << endl;

	return mMoves;
}

void MoveGenerator::addRay(bool *ray, int startSquare, int targetSquare, int direction) {
	int numStep = (targetSquare - startSquare) / direction + 1;

	for (int i = 0; i < numStep; i++)
		ray[startSquare + direction * i] = true;
}

void MoveGenerator::generateSlidingAttackMap() {
	for (int i = 0; i < mpBoard->queens[opponentColourIndex]->count(); i++)
		generateSlidingPieceAttackMap((*mpBoard->queens[opponentColourIndex])[i], 0, 8);

	for (int i = 0; i < mpBoard->rooks[opponentColourIndex]->count(); i++)
		generateSlidingPieceAttackMap((*mpBoard->rooks[opponentColourIndex])[i], 0, 4);

	for (int i = 0; i < mpBoard->bishops[opponentColourIndex]->count(); i++)
		generateSlidingPieceAttackMap((*mpBoard->bishops[opponentColourIndex])[i], 4, 8);
}

void MoveGenerator::generateSlidingPieceAttackMap(int startSquare, int startIndex, int endIndex) {
	for (int dirIndex = startIndex; dirIndex < endIndex; dirIndex++) {
		int offsetDirection = mPrecomputedMoveData.directionOffsets[dirIndex];

		for (int i = 0; i < mPrecomputedMoveData.numSquaresToEdge[startSquare][dirIndex]; i++) {
			int targetSquare = startSquare + offsetDirection * (i + 1);
			int targetPiece = mpBoard->getPiece(targetSquare);

			attackedSquares[targetSquare] = true;

			if (Piece::pieceType(targetPiece) != Piece::none && targetPiece != (Piece::king | friendlyColour))
				break;
		}
	}
}


void MoveGenerator::generateAttackData() {
	// Generating attack map for Queens, Rooks and bishops
	generateSlidingAttackMap();

	// Generate check ray map
	int startIndex = 0;
	int endIndex = 8;

	if (mpBoard->queens[opponentColourIndex]->count() == 0) {
		startIndex = mpBoard->rooks[opponentColourIndex]->count() > 0 ? 0 : 4;
		endIndex = mpBoard->bishops[opponentColourIndex]->count() > 0 ? 8 : 4;
	}

	for (int dir = startIndex; dir < endIndex; dir++) {
		int startSquare = mpBoard->kings[friendlyColourIndex];

		bool isDiagonal = dir > 3;

		int numSquare = mPrecomputedMoveData.numSquaresToEdge[startSquare][dir];
		int dirOffset = mPrecomputedMoveData.directionOffsets[dir];

		bool isFriendlyPieceInRay = false;

		for (int i = 0; i < numSquare; i++) {
			int targetSquare = startSquare + dirOffset * (i + 1);
			int piece = mpBoard->getPiece(targetSquare);

			// If square is occupied
			if (piece != Piece::none) {
				// If friendly piece, analyze for pins
				if (Piece::isColour(piece, friendlyColour)) {
					// If first ally piece, might be pinned
					if (!isFriendlyPieceInRay) {
						isFriendlyPieceInRay = true;
					} 
					// If 2 ally pieces in row, no check nor pin
					else {
						break;
					}
				}
				// If ennemy piece, analyse for checks and pins
				else {
					// If ennemy piece is able to attack the king
					if ((!isDiagonal && Piece::hasStraightAttack(piece)) || (isDiagonal && Piece::hasDiagonalAttack(piece))) {
						// If there is a friendly piece in the way, add to the pin ray map
						if (isFriendlyPieceInRay) {
							addRay(pinRaySquares, startSquare, targetSquare, dirOffset);
						}
						// If no friendly piece in the way, add to the check ray map
						else {
							addRay(checkRaySquares, startSquare, targetSquare, dirOffset);

							inDoubleCheck = inCheck;
							inCheck = true;
						}
						break;
					}
					// Piece can't attack the king and block further attack
					else {
						break;
					}
				}
			}
		}
	}

	// Generating king attacks
	int opponentKingSquare = mpBoard->kings[opponentColourIndex];
	for (int targetSquare : mPrecomputedMoveData.kingMoves[opponentKingSquare]) {
		attackedSquares[targetSquare] = true;
	}

	// Generating knight attacks
	for (int i = 0; i < mpBoard->knights[opponentColourIndex]->count(); i++) {
		int startSquare = (*mpBoard->knights[opponentColourIndex])[i];

		for (int targetSquare : mPrecomputedMoveData.knightMoves[startSquare]) {
			attackedSquares[targetSquare] = true;

			if (mpBoard->kings[friendlyColourIndex] == targetSquare) {
				checkRaySquares[startSquare] = true;

				inDoubleCheck = inCheck;
				inCheck = true;
			}
		}
	}
	
	// Generating pawn attacks (without en-passant attacks)
	int pawnOffset = isWhiteToMove ? -8 : 8;
	for (int i = 0; i < mpBoard->pawns[opponentColourIndex]->count(); i++) {
		for (int j = 0; j < 2; j++) {
			int startSquare = (*mpBoard->pawns[opponentColourIndex])[i];

			if (mPrecomputedMoveData.numSquaresToEdge[startSquare][mPrecomputedMoveData.pawnAttackDirections[opponentColourIndex][j]] > 0) {
				int pawnCaptureDir = mPrecomputedMoveData.directionOffsets[mPrecomputedMoveData.pawnAttackDirections[opponentColourIndex][j]];
				int targetSquare = startSquare + pawnCaptureDir;

				attackedSquares[targetSquare] = true;

				if (mpBoard->kings[friendlyColourIndex] == targetSquare) {
					checkRaySquares[startSquare] = true;

					inDoubleCheck = inCheck;
					inCheck = true;
				}
			}
		}
	}
}

void MoveGenerator::generateKingMoves() {
	int startSquare = mpBoard->kings[friendlyColourIndex];

	for (int targetSquare : mPrecomputedMoveData.kingMoves[startSquare]) {
		if (Piece::isColour(mpBoard->getPiece(targetSquare), friendlyColour))
			continue;

		bool isCapture = Piece::isColour(mpBoard->getPiece(targetSquare), opponentColour);

		if (mCapturedOnly && !isCapture)
			continue;

		// If safe square to move
		if (!isAttackedSquare(targetSquare)) {
			mMoves.push_back(Move(startSquare, targetSquare));

			// Handle castle situations
			if (!inCheck && !isCapture) {
				// King side (5 = f1 / 61 = f8)
				if ((targetSquare == 5 || targetSquare == 61) && this->hasKingSideCastleRight()) {
					int castleSquare = targetSquare + 1;

					if (Piece::pieceType(mpBoard->getPiece(castleSquare)) == Piece::none) {
						if (!isAttackedSquare(castleSquare)) {
							mMoves.push_back(Move(startSquare, castleSquare, Move::Flag::castling));
						}
					}

				}

				// Queen side (3 = d1 / d59 = 8)
				if ((targetSquare == 3 || targetSquare == 59) && this->hasQueenSideCastleRight()) {
					int castleSquare = targetSquare - 1;

					if ((Piece::pieceType(mpBoard->getPiece(castleSquare)) == Piece::none)
						&& (Piece::pieceType(mpBoard->getPiece(castleSquare - 1)) == Piece::none)) {
						if (!isAttackedSquare(castleSquare)) {
							mMoves.push_back(Move(startSquare, castleSquare, Move::Flag::castling));
						}
					}
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
	int epFile = ((int)(mpBoard->currentGameState >> 4) & 0b1111) - 1;
	int epSquare = -1;
	if (epFile != -1) {
		epSquare = 8 * ((mpBoard->whiteToMove) ? 5 : 2) + epFile;
	}
	
	for (int i = 0 ; i < mpBoard->pawns[friendlyColourIndex]->count(); i++) {
		int startSquare = (*mpBoard->pawns[friendlyColourIndex])[i];
		int pawnRank = startSquare / 8;
		bool oneRankToPromotion = pawnRank == finalRankBeforePromotion;

		if (!mCapturedOnly) {
			int squareForward = startSquare + offset;

			if (mpBoard->getPiece(squareForward) == Piece::none) {
				// If pawn isn't pinned or moving in the check ray
				if (!isPinned(startSquare) || isMovingInRay(startSquare, mpBoard->kings[friendlyColourIndex], offset)) {

					// If king not in check or preventing the check
					if (!inCheck || isInCheckRay(squareForward)) {
						// If reaching the top, adding promotions moves
						if (oneRankToPromotion) {
							makePromotionMoves(startSquare, squareForward);
						}
						else {
							mMoves.push_back(Move(startSquare, squareForward));
						}
					}

					// Handeling starting position (2 square forward moves)
					if ((pawnRank) == startRank) {
						int squareTwoForward = squareForward + offset;

						if (mpBoard->getPiece(squareTwoForward) == Piece::none) {
							// If king not in check or preventing the check
							if (!inCheck || isInCheckRay(squareTwoForward)) {
								mMoves.push_back(Move(startSquare, squareTwoForward, Move::Flag::pawnTwoForward));
							}
						}
					}
				}
			}
		}

		// Handeling pawn capture
		for (int j = 0; j < 2; j++) {
			if (mPrecomputedMoveData.numSquaresToEdge[startSquare][mPrecomputedMoveData.pawnAttackDirections[friendlyColourIndex][j]] > 0) {
				int pawnCaptureDir = mPrecomputedMoveData.directionOffsets[mPrecomputedMoveData.pawnAttackDirections[friendlyColourIndex][j]];
				int targetSquare = startSquare + pawnCaptureDir;
				int targetPiece = mpBoard->getPiece(targetSquare);

				// If is pinned and not moving in the check ray skip move
				if (isPinned(startSquare) && !isMovingInRay(startSquare, mpBoard->kings[friendlyColourIndex], pawnCaptureDir)) {
					continue;
				}

				// Regular capture
				if (Piece::isColour(targetPiece, opponentColour)) {
					// If king in check and not defending it, skip this move
					if (inCheck && !isInCheckRay(targetSquare))
						continue;

					if (oneRankToPromotion)
						makePromotionMoves(startSquare, targetSquare);
					else
						mMoves.push_back(Move(startSquare, targetSquare));
				}

				// Capture en-passant
				if (targetSquare == epSquare) {
					if (!isInCheckAfterEnPassant(startSquare, targetSquare - offset))
						if (!inCheck || isInCheckRay(targetSquare - offset))
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
	bool pinned = isPinned(startSquare);

	// If king in check and piece is pinned, then this piece can't move
	if (inCheck && pinned) {
		return;
	}

	for (int dirIndex = startIndex; dirIndex < endIndex; dirIndex++) {
		int offsetDirection = mPrecomputedMoveData.directionOffsets[dirIndex];

		// If the piece is pinned, it can only move in the check ray
		if (pinned && !isMovingInRay(startSquare, mpBoard->kings[friendlyColourIndex], offsetDirection))
			continue;

		for (int i = 0; i < mPrecomputedMoveData.numSquaresToEdge[startSquare][dirIndex]; i++) {
			int targetSquare = startSquare + offsetDirection * (i + 1);
			int targetPiece = mpBoard->getPiece(targetSquare);

			// If friendly colour, stop looking further in that direction
			if (Piece::isColour(targetPiece, friendlyColour))
				break;

			// Add the move if king not in check or if it prevents check
			bool preventCheck = isInCheckRay(targetSquare);
			if (!inCheck || preventCheck) {
				if (!mCapturedOnly || targetPiece != Piece::none) {
					mMoves.push_back(Move(startSquare, targetSquare));
				}
			}

			// If captured piece or if the move prevent check stop looking further in that direction 
			if (Piece::isColour(targetPiece, opponentColour) || preventCheck)
				break;	
		}
	}
}

void MoveGenerator::generateKnightMoves() {
	for (int i = 0; i < mpBoard->knights[friendlyColourIndex]->count(); i++) {
		int startSquare = (*mpBoard->knights[friendlyColourIndex])[i];

		// If pinned, knight can't move
		if (isPinned(startSquare)) {
			continue;
		}

		for (int targetSquare : mPrecomputedMoveData.knightMoves[startSquare]) {
			int targetPiece = mpBoard->getPiece(targetSquare);

			// If friendly colour, skip the move
			if (Piece::isColour(targetPiece, friendlyColour))
				continue;

			// If capture only, skip if not captured piece
			if (mCapturedOnly && targetPiece == Piece::none)
				continue;

			// If in check and knight moving to a check ray, skip the move
			if (inCheck && !isInCheckRay(targetSquare))
				continue;

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

bool MoveGenerator::isInCheckAfterEnPassant(int square, int enPassantSquare) {
	int kingSquare = mpBoard->kings[friendlyColourIndex];
	int dir = mPrecomputedMoveData.directionTable[square - kingSquare + 63];

	// If pawn in same rank as king, then check is not possible
	if (dir != 1 && dir != -1)
		return false;

	// Checking if king in check by sliding piece (rook or queen)
	int dirIndex = (dir == 1) ? 3 : 2;
	int numToEdge = mPrecomputedMoveData.numSquaresToEdge[kingSquare][dirIndex];
	for (int i = 0; i < numToEdge; i++) {
		int targetSquare = kingSquare + dir * (i + 1);

		// Ignoring capturing piece and capture piece as it won't be on the rank anymore 
		if (targetSquare == square || targetSquare == enPassantSquare)
			continue;

		// If square is occupied by friendly piece, check is not possible
		int piece = mpBoard->getPiece(targetSquare);
		if (Piece::isColour(piece, friendlyColour))
			return false;

		// If no piece, keep looking for next piece
		if (piece == Piece::none)
			continue;

		// If is a rook or queen (straight attack) of opponent piece, then there is a check
		if (Piece::hasStraightAttack(piece))
			return true;
		else
			break;
	}

	return false;
}

bool MoveGenerator::isMovingInRay(int startquare, int targetSquare, int direction) {
	int moveDirection = mPrecomputedMoveData.directionTable[targetSquare - startquare + 63];
	return direction == moveDirection || -direction == moveDirection;
}

bool MoveGenerator::isInCheckRay(int square) {
	return checkRaySquares[square];
}

bool MoveGenerator::isPinned(int square) {
	return pinRaySquares[square];
}

bool MoveGenerator::isAttackedSquare(int square) {
	return attackedSquares[square];
}