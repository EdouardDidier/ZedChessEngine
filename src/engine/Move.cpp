#include "Move.h"

Move::Move(Uint16 moveValue) {
	this->moveValue = moveValue;
}

Move::Move(int startSquare, int targetSquare) {
	moveValue = (Uint16)(startSquare | targetSquare << 6);
}

Move::Move(int startSquare, int targetSquare, int flag) {
	moveValue = (Uint16)(startSquare | targetSquare << 6 | flag << 12);
}

int Move::getStartSquare() const {
	return moveValue & startSquareMask;
}

int Move::getTargetSquare() const {
	return (moveValue & targetSquareMask) >> 6;
}

int Move::getFlag() const {
	return moveValue >> 12;
}

bool Move::isEnPassant() const {
	return this->getFlag() == Move::Flag::enPassantCapture;
}

bool Move::isCastle() const {
	return this->getFlag() == Move::Flag::castling;
}

bool Move::isPromotion() const {
	int flag = this->getFlag();
	return flag == Move::Flag::promoteToQueen || flag == Move::Flag::promoteToRook || flag == Move::Flag::promoteToBishop || flag == Move::Flag::promoteToKnight;
}

Move Move::getCastleRookMove(Move move) {
	if (!move.isCastle())
		return Move(0);

	int rookStartSquare = move.getTargetSquare();
	int rookTargetSquare = rookStartSquare;

	// White king side 6 = g1
	// Black king side 62 = f8
	if (rookStartSquare == 6 || rookStartSquare == 62) {
		rookStartSquare += 1;
		rookTargetSquare -= 1;
	}

	// White queen side 2 = c1
	// Black queen side 58 = c8
	else if (rookStartSquare == 2 || rookStartSquare == 58) {
		rookStartSquare -= 2;
		rookTargetSquare += 1;
	}
	else return Move(0);

	return Move(rookStartSquare, rookTargetSquare);
}

bool Move::isSameMove(Move a, Move b) {
	return a.moveValue == b.moveValue;
}

bool Move::isSameMoveNoFlag(Move a, Move b) {
	return (a.moveValue & Move::noFlagMask) == (b.moveValue & Move::noFlagMask);
}

Move Move::invalidMove() {
	return Move(0);
}

bool Move::isInvalid() {
	return moveValue == 0;
}