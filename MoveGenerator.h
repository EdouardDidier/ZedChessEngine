#pragma once

#include <iostream>
#include <list>

#include "PreComputedMoveData.h"

#include "Board.h"
#include "Piece.h"
#include "Move.h"

#include "Profiler.h"

using namespace std;

class MoveGenerator
{
public:
	bool isWhiteToMove;
	int friendlyColour;
	int opponentColour;
	int friendlyColourIndex;
	int opponentColourIndex;

	MoveGenerator();
	~MoveGenerator();

	void init();

	list<Move> generateLegalMove(Board* pBoard);

	void generateAttackMoves();

	void generateKingMoves();
	void generatePawnMove();
	void generateSlidingMoves();
	void generateSlidingPieceMoves(int startSquare, int startIndex, int endIndex);
	void generateKnightMoves();

	void makePromotionMoves(int fromSquare, int toSquare);


	bool hasKingSideCastleRight();
	bool hasQueenSideCastleRight();

private:
	PreComputedMoveData mPrecomputedMoveData;

	list<Move> mMoves;	//TODO: passer en pointer de moves ?
	Board* mpBoard;

	Profiler mProfiler;
};

