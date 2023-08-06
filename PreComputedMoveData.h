#pragma once

#include <iostream>
#include <vector>

using namespace std;

class PreComputedMoveData //TODO rendre cette class static
{
public:
	// First 4 are orthogonal, last 4 are diagonals (N, S, W, E, NW, SE, NE, SW)
	const int directionOffsets[8] = { 8, -8, -1, 1, 7, -7, 9, -9 };

	const int knightOffsets[8] = { 15, 17, -17, -15, 10, -6, 6, -10 };

	// Stores number of moves available in each of the 8 directions for every square on the board
	// Order of directions is: N, S, W, E, NW, SE, NE, SW
	int numSquaresToEdge[64][8];

	// Pawn attack directions for white and black (NW, NE; SW SE)
	short pawnAttackDirections[2][2] = {
		{ 4, 6 },
		{ 7, 5 }
	};

	// Array that store all knight and kingMoves possible moves from each square
	vector<int> knightMoves[64];
	vector<int> kingMoves[64];


	PreComputedMoveData();
	~PreComputedMoveData();

	void init();
};
