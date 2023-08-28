#include "PieceList.h"

#include <iostream>

PieceList::PieceList(int maxPieceNum) {
	mOccupiedSquares = new int[maxPieceNum];
	mSquareToOccupied = new int[64];

	mNumPiece = 0;
}

PieceList::~PieceList() {
	delete mOccupiedSquares;
	delete mSquareToOccupied;
}

int PieceList::count() {
	return mNumPiece;
}

void PieceList::addAtSquare(int square) {
	mOccupiedSquares[mNumPiece] = square;
	mSquareToOccupied[square] = mNumPiece;
	mNumPiece++;
}

void PieceList::removeAtSquare(int square) {
	int pieceIndex = mSquareToOccupied[square];
	mOccupiedSquares[pieceIndex] = mOccupiedSquares[mNumPiece - 1];
	mSquareToOccupied[mOccupiedSquares[pieceIndex]] = pieceIndex;

	mNumPiece--;
}

void PieceList::movePiece(int startSquare, int targetSquare) {
	mOccupiedSquares[mSquareToOccupied[startSquare]] = targetSquare;
	mSquareToOccupied[targetSquare] = mSquareToOccupied[startSquare];
}

int PieceList::operator [](int i) const {
	return mOccupiedSquares[i];
}