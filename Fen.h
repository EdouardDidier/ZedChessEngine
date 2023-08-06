#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <map>

#include "Piece.h"

using namespace std;

class Fen
{
public:
	class Position {
	public:
		int squares[64];

		bool whiteCastleKingside;
		bool whiteCastleQueenside;
		bool blackCastleKingside;
		bool blackCastleQueenside;

		int epFile;

		bool whiteToMove;

		int fiftyMoveCounter;
		int moveCount;
	};

	Fen();
	~Fen();

	static const string fenStartPosition;

	static Position positionFromFen(string fen);

	// Mapping char to piece id
	static map<char, int> symbolToPiece;
};

