#pragma once

#include <iostream>
#include <list>

#include "PieceList.h"
#include "Move.h"

#include "Zobrist.h"
#include "Fen.h"
#include "Coord.h"

using namespace std;

class Zobrist;

class Board
{
public:
	static const int whiteIndex = 0;
	static const int blackIndex = 1;

	int* squares;

	bool whiteToMove;		
	int colourToMove;
	int opponentColour;
	int colourToMoveIndex;
	int opponentColourIndex;

	// Bits 0-3 store white and black kingside/queenside castling legality
	// Bits 4-7 store file of ep square (starting at 1, so 0 = no ep square)
	// Bits 8-13 captured piece
	// Bits 14-... fifty mover counter
	Uint32 currentGameState;
	list<Uint32> gameStateHistory;
	list<Move> moveHistory;
	list<Move> moveToRedo;

	Uint64 zobristKey;
	list<Uint64> zobristKeyHistory;
	list<Uint64> repetitionHistory; //TODO: probably create a history of repetition history for undoMove

	int moveCount; // Total full move played in game
	int fiftyMoveCounter; // Number of halfmove last pawn move or capture

	// Arrays to store king location
	int *kings;

	// List of piece to avoid looping through the board while generating moves
	PieceList **queens;
	PieceList **rooks;
	PieceList **bishops;
	PieceList **knights;
	PieceList **pawns;

	const Uint16 whiteCastleKingSideMask = 0b1111111111111110;
	const Uint16 whiteCastleQueenSideMask = 0b1111111111111101;
	const Uint16 blackCastleKingSideMask = 0b1111111111111011;
	const Uint16 blackCastleQueenSideMask = 0b1111111111110111;

	const Uint16 whiteCastleMask = whiteCastleKingSideMask & whiteCastleQueenSideMask;
	const Uint16 blackCastleMask = blackCastleKingSideMask & blackCastleQueenSideMask;

private:
	PieceList** mAllPieceLists;

public:
	Board();
	~Board();

	void makeMove(Move move, bool eraseMoveToRedo = true);
	void redoMove();
	void undoMove();

	Move getFirstRedoMove();
	Move getLastMove();

	int getPiece(int index);
	int getPiece(int fileIndex, int rankIndex);
	int getPiece(Coord coord);

	PieceList *getPieceList(int pieceType, int pieceColour);

	bool isRepetition(int maxCount = 3);

	void reset();

	void loadStartPosition();
	void loadPosition(string fen);

private:
	void createPieceArrays();
	void deletePieceArrays();

};