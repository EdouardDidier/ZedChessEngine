#include "Piece.h"

const int Piece::simpleValue[] = {
	0, 0, 1, 3, 0, 3, 5, 9
};

Piece::Piece() {

}

Piece::~Piece() {

}

bool Piece::isColour(int piece, int colour) {
	return (piece & colourMask) == colour;
}

int Piece::colour(int piece) {
	return piece & colourMask;
}

int Piece::pieceType(int piece) {
	return piece & typeMask;
}

bool Piece::hasStraightAttack(int piece) {
	return (piece & 0b110) == 0b110;
}

bool Piece::hasDiagonalAttack(int piece) {
	return (piece & 0b101) == 0b101;
}

bool Piece::isSlidingPiece(int piece) {
	return (piece & 0b100) != 0;
}