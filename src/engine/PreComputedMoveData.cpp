#include "PreComputedMoveData.h"

PreComputedMoveData::PreComputedMoveData() {
	for (int squareIndex = 0; squareIndex < 64; squareIndex++) {
		// Calculate all number of squares to edge in each direction
		int y = squareIndex / 8;
		int x = squareIndex % 8;

		int north = 7 - y;
		int south = y;
		int west = x;
		int east = 7 - x;

		numSquaresToEdge[squareIndex][0] = north;
		numSquaresToEdge[squareIndex][1] = south;
		numSquaresToEdge[squareIndex][2] = west;
		numSquaresToEdge[squareIndex][3] = east;
		numSquaresToEdge[squareIndex][4] = min(north, west);
		numSquaresToEdge[squareIndex][5] = min(south, east);
		numSquaresToEdge[squareIndex][6] = min(north, east);
		numSquaresToEdge[squareIndex][7] = min(south, west);

		// Calculate all possible knight move
		for (int knightOffset : knightOffsets) {
			int knightTargetSquare = squareIndex + knightOffset;
			if (knightTargetSquare >= 0 && knightTargetSquare < 64) {
				// Check if warp up, then discard the move
				int knightX = knightTargetSquare % 8;

				// Add move if no warp around the board
				if (abs(knightX - x) < 3 )
					knightMoves[squareIndex].push_back(knightTargetSquare);
			}
		}

		// Calculate all possible king move
		for (int kingOffset : directionOffsets) {
			int kingTargetSquare = squareIndex + kingOffset;
			if (kingTargetSquare >= 0 && kingTargetSquare < 64) {
				// Check if warp up, then discard the move
				int kingX = kingTargetSquare % 8;

				// Add move if no warp around the board
				if (abs(kingX - x) < 3)
					kingMoves[squareIndex].push_back(kingTargetSquare);
			}
		}
	}

	// Calculate direction table (return direction given a difference of 2 square index (+63)
	for (int i = 0; i < 127; i++) {
		int offset = i - 63;
		int sign = offset < 0 ? -1 : 1;
		
		if (!(offset % 7))
			directionTable[i] = sign * 7;
		else if (!(offset % 8))
			directionTable[i] = sign * 8;
		else if (!(offset % 9))
			directionTable[i] = sign * 9;
		else
			directionTable[i] = sign;
	}
}

PreComputedMoveData::~PreComputedMoveData() {

}

void PreComputedMoveData::init() {

}