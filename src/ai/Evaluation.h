#pragma once

#include "PieceSquareTables.h"

#include "./src/engine/Board.h"
#include "./src/engine/PieceList.h"


class EvaluationV5
{
public:	
	static const int maxEval = 100000;
	static const int minEval = -100000;

	static const int pawnValue = 100;
	static const int knightValue = 300;
	static const int bishopValue = 320;
	static const int rookValue = 500;
	static const int queenValue = 900;

	static const int capturedMultiplier = 10;
	static const int squareControlledPenality = 350;

public:
	EvaluationV5();

	int evaluate(Board *pBoard);
	int evaluatePieceSquareTables();
	int evaluatePieceSquare(const int* table, PieceList *pieceList, bool isWhite);

private:
	Board *mpBoard;
	
	int mFriendlyIndex;
	int mOpponentIndex;
};

