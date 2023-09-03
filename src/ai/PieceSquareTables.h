#pragma once

#include "./src/graphics/Coord.h"

class PieceSquareTables
{
public:
	static int getSquareValue(const int *table, int square, bool isWhite);

public:
	static const int pawn[];
	static const int knight[];
	static const int bishop[];
	static const int rook[];
	static const int queen[];
	static const int king_mg[];
	static const int king_eg[];
};

