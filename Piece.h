#pragma once

class Piece
{
public:
	Piece();
	~Piece();

	// Defening piece id
	static const int none = 0;		// 0b000
	static const int king = 1;		// 0b001
	static const int pawn = 2;		// 0b010
	static const int knight = 3;	// 0b011
	static const int bishop = 5;	// 0b101
	static const int rook = 6;		// 0b110
	static const int queen = 7;		// 0b101

	static const int white = 8;
	static const int black = 16;

	// Creating mask to filter easily on specifics datas
	static const int typeMask = 0b00111;
	static const int whiteMask = 0b01000;
	static const int blackMask = 0b10000;
	static const int colourMask = whiteMask | blackMask;

	// Genric Method to get pieces datas
	static bool isColour(int piece, int colour);
	static int colour(int piece);
	static int pieceType(int piece);
	static bool hasStraightAttack(int piece);
	static bool hasDiagonalAttack(int piece);
	static bool isSlidingPiece(int piece);
};

