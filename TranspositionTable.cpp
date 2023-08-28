#include "TranspositionTable.h"

TranspositionTable::TranspositionTable() {
	mpBoard = NULL;

	mTable = new TableEntry[size];
}

TranspositionTable::~TranspositionTable() {
	delete mTable;
}

void TranspositionTable::init(Board* pBoard) {
	mpBoard = pBoard;
}

void TranspositionTable::storeEvaluation(int v, int d, TableEntryType t, Move m) {
	int index = mpBoard->zobristKey % size;
	
	if (mTable[index].type == invalid)	{
		stats.numWrites++;
		mTable[index] = TableEntry(mpBoard->zobristKey, t, v, d, m);
		return;
	}
	
	if (d > mTable[index].depth) {
		stats.numRewrites++;
		mTable[index] = TableEntry(mpBoard->zobristKey, t, v, d, m);
	}
}

int TranspositionTable::getEvaluation(int alpha, int beta, int d) {
	TableEntry entry = mTable[mpBoard->zobristKey % size];

	if (entry.type == invalid)
		return invalidEval;

	if (entry.zobrist != mpBoard->zobristKey) {
		/*cout << "Entry datas: " << entry.zobrist % size << " | " << entry.zobrist << " % " << size << endl;
		cout << "Entry datas: " << mpBoard->zobristKey % size << " | " << mpBoard->zobristKey << " % " << size << endl;*/
		stats.numMisses++; 
		return invalidEval;
	}

	if (entry.depth < d)
		return invalidEval;

	if (entry.type == exact) {
		stats.numTranspositions++;
		return entry.value;
	}

	if (entry.type == upperBound && entry.value <= alpha) {
		stats.numTranspositions++;
		return entry.value;
	}

	if (entry.type == lowerBound && entry.value >= beta) {
		stats.numTranspositions++;
		return entry.value;
	}

	return invalidEval;
}

Move TranspositionTable::getMove() {
	return mTable[mpBoard->zobristKey % size].move;
}

TranspositionTable::TableEntry TranspositionTable::getEntry() {
	return mTable[mpBoard->zobristKey % size];
}

void TranspositionTable::clear() {
	for (int i = 0; i < size; i++)
		mTable[i] = TableEntry();
}

void TranspositionTable::resetStats() {
	stats = TableStats();
}

TranspositionTable::TableEntry::TableEntry()
	: zobrist(0), type(invalid), value(0), depth(0), move(0) {}

TranspositionTable::TableEntry::TableEntry(Uint64 zKey, TableEntryType t, int v, int d, Move m)
	: zobrist(zKey), type(t), value(v), depth(d), move(m) {}