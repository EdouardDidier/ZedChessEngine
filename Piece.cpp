#include "Piece.h"

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

bool Piece::isRookOrQueen(int piece) {
	return (piece & 0b110) == 0b110;
}

bool Piece::isBishopOrQueen(int piece) {
	return (piece & 0b101) == 0b101;
}

bool Piece::isSlidingPiece(int piece) {
	return (piece & 0b100) != 0;
}