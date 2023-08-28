#pragma once

#include <iostream>

class PieceList
{
public:
	PieceList(int maxPieceNum = 16);
	~PieceList();

	int count();
	
	void addAtSquare(int square);
	void removeAtSquare(int square);
	void movePiece(int startSquare, int targetSquare);

	int operator [](int i) const;
	
private:
	int *mOccupiedSquares;
	int *mSquareToOccupied;

	int mNumPiece;
};

