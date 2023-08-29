#pragma once

#include <iostream>
#include <list>

#include "PreComputedMoveData.h"

#include "Board.h"
#include "Piece.h"
#include "Move.h"

#include "./src/utility/Profiler.h"

using namespace std;

class MoveGenerator
{
public:
	bool isWhiteToMove;
	int friendlyColour;
	int opponentColour;
	int friendlyColourIndex;
	int opponentColourIndex;

	bool inCheck;
	bool inDoubleCheck;
	bool attackedSquares[64]; //TODO: remplacer par bitmask
	bool checkRaySquares[64]; //TODO: remplacer par bitmask
	bool pinRaySquares[64]; //TODO: remplacer par bitmask

	MoveGenerator();
	~MoveGenerator();

	void init();
	void resetAttackData();

	vector<Move> generateLegalMove(Board* pBoard, bool capturedOnly = false);

	void addRay(bool *ray, int startSquare, int targetSquare, int direction);
	void generateSlidingAttackMap();
	void generateSlidingPieceAttackMap(int startSquare, int startIndex, int endIndex);
	void generateAttackData();

	void generateKingMoves();
	void generatePawnMove();
	void generateSlidingMoves();
	void generateSlidingPieceMoves(int startSquare, int startIndex, int endIndex);
	void generateKnightMoves();

	void makePromotionMoves(int fromSquare, int toSquare);

	bool hasKingSideCastleRight();
	bool hasQueenSideCastleRight();
	bool isInCheckAfterEnPassant(int square, int enPassantSquare);

	bool isMovingInRay(int startSquare, int targetSquare, int direction);
	bool isInCheckRay(int square);
	bool isPinned(int square);
	bool isAttackedSquare(int square);

private:
	PreComputedMoveData mPrecomputedMoveData;

	vector<Move> mMoves;
	Board* mpBoard;

	Profiler mProfiler;

	bool mCapturedOnly;
};

