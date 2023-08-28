#pragma once

#include <SDL.h>

#include <iostream>
#include <random>

#include "./src/engine/Board.h"

using namespace std;

class Board;

class Zobrist
{
public:
	// Zobrist key for move to play
	static Uint64 sideToMoveKey;

	// Zobrist keys for pieces (2 colors, 6 pieces, wasting 2 dimensions for efficienty, matching piece values, 64 squares)
	static Uint64 pieceKeys[2][8][64];

	// Zobrist keys for castle rights
	static Uint64 castleKeys[16];

	// Zobrist keys for ep
	static Uint64 epKeys[8];

	static void init();
	static Uint64 calculateKey(Board *pBoard);

private:
	static mt19937_64 mGen64;
};

