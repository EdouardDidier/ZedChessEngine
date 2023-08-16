#include "Zobrist.h"

Uint64 Zobrist::sideKey;
Uint64 Zobrist::pieceKeys[2][8][64];
Uint64 Zobrist::casleKeys[16];
Uint64 Zobrist::epKeys[8];
mt19937_64 Zobrist::mGen64;

void Zobrist::init() {
	mGen64.seed(mGen64.default_seed);

	cout << "Initializing Zobrist keys..." << endl;

	// Initializing side key
	sideKey = mGen64();

	// Initializing piece keys
	for (int colourIndex = 0; colourIndex < 2; colourIndex++) {
		for (int pieceIndex = 0; pieceIndex < 8; pieceIndex++) {
			for (int squareIndex = 0; squareIndex < 64; squareIndex++) {
				pieceKeys[colourIndex][pieceIndex][squareIndex] = mGen64();
			}
		}
	}

	// Initializing castle keys
	for (int i = 0; i < 16; i++) {
		casleKeys[i] = mGen64();
	}

	// Initializing ep keys
	for (int i = 0; i < 8; i++) {
		epKeys[i] = mGen64();
	}

	cout << "Zobrist keys initialized" << endl;
}


Uint64 Zobrist::calculateKey(Board *pBoard) {
	Uint64 key = 0;

	if (!pBoard->whiteToMove)
		key ^= sideKey;

	for (int i = 0; i < 64; i++) {
		int piece = pBoard->getPiece(i);
		int colourIndex = Piece::isColour(piece, Piece::white) ? Board::whiteIndex : Board::blackIndex;

		key ^= pieceKeys[colourIndex][Piece::pieceType(piece)][i];
	}

	// Castle
	key ^= casleKeys[pBoard->currentGameState & 0b1111];

	// Ep
	int epIndex = ((pBoard->currentGameState >> 4) & 0b1111) - 1;
	if (epIndex > -1 && epIndex < 8)
		key ^= epKeys[epIndex];

	return key;
}
