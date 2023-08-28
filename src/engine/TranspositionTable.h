#pragma once

#include <SDL.h>

#include "Board.h"
#include "Move.h"

class TranspositionTable
{
public:			
	enum TableEntryType {
		invalid, exact, upperBound, lowerBound
	};

	class TableEntry
	{
	public:
		Uint64 zobrist;
		TableEntryType type;
		int value;
		int depth;
		Move move;

	public:
		TableEntry();
		TableEntry(Uint64 zKey, TableEntryType t, int v, int d, Move m);
	};

	struct TableStats {
		int numMisses = 0;
		int numTranspositions = 0;
		int numWrites = 0;
		int numRewrites = 0;
	};

public:
	static const int size = 1000000; // Random start size (2^20)
	static const int invalidEval = INT_MIN + 1;

	TableStats stats;

private:
	Board *mpBoard;
	TableEntry *mTable;

public:
	TranspositionTable();
	~TranspositionTable();

	void init(Board* pBoard);

	void storeEvaluation(int v, int d, TableEntryType t, Move m);
	int getEvaluation(int alpha, int beta, int d);

	Move getMove();
	TableEntry getEntry();

	void clear();
	void resetStats();
};

