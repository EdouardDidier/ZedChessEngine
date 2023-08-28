#pragma once

#include "./src/engine/Board.h"

class EvaluationV4
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
	int evaluate(Board *pBoard);
};

