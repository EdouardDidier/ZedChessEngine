#pragma once

#include "Board.h"

class EvaluationV2
{
public:
	const int pawnValue = 100;
	const int knightValue = 300;
	const int bishopValue = 320;
	const int rookValue = 500;
	const int queenValue = 900;

	int evaluate(Board* pBoard);

};

