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

class EvaluationV3
{
public:
	static const int maxEval = 100000;
	static const int minEval = -100000;

	static const int pawnValue = 100;
	static const int knightValue = 300;
	static const int bishopValue = 320;
	static const int rookValue = 500;
	static const int queenValue = 900;

public:
	int evaluate(Board* pBoard);
};

