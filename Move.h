#pragma once

#include <SDL.h>

/*
To preserve memory during search, moves are stored as 16 bit numbers.
The format is as follows:

bit 0-5: from square (0 to 63)
bit 6-11: to square (0 to 63)
bit 12-15: flag 
*/

class Move
{
public:
	const struct Flag {
		static const int none = 0;
		static const int enPassantCapture = 1;
		static const int castling = 2;
		static const int promoteToQueen = 3;
		static const int promoteToKnight = 4;
		static const int promoteToRook = 5;
		static const int promoteToBishop = 6;
		static const int pawnTwoForward = 7;
	};

	static const Uint16 startSquareMask = 0b0000000000111111;
	static const Uint16 targetSquareMask = 0b0000111111000000;
	static const Uint16 flagMask = 0b1111000000000000;
	static const Uint16 noFlagMask = 0b0000111111111111;

	Uint16 moveValue;

	Move(Uint16 moveValue = 0);
	Move(int startSquare, int targetSquare);
	Move(int startSquare, int targetSquare, int flag);

	int getStartSquare();
	int getTargetSquare();

	int getFlag();

	bool isEnPassant();
	bool isCastle();
	bool isPromotion();

	static Move getCastleRookMove(Move move);

	static bool isSameMove(Move a, Move b);
	static bool isSameMoveNoFlag(Move a, Move b);

	static Move invalidMove();
	bool isInvalid();
};

