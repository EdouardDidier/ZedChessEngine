#include "TranspositionTable.h"

TranspositionTable::TranspositionTable() {
	mpBoard = NULL;

	mSize = mbSize * 1024 * 1024 / sizeof(TableEntry); // TODO, change the size to be in megabyte instead of entry

	mTable = new TableEntry[mSize]; 
}

TranspositionTable::~TranspositionTable() {
	delete[] mTable;
}

void TranspositionTable::init(Board* pBoard) {
	mpBoard = pBoard;
}

void TranspositionTable::storeEvaluation(int v, int d, TableEntryType t, Move m) {
	int index = mpBoard->zobristKey % mSize;
	
	if (mTable[index].type == invalid)	{
		stats.numWrites++;
		mTable[index] = TableEntry(mpBoard->zobristKey, t, v, d, m);
		return;
	}
	
	if (d > mTable[index].depth) {
		stats.numOverwrites++;
		mTable[index] = TableEntry(mpBoard->zobristKey, t, v, d, m);
	}
}

int TranspositionTable::getEvaluation(int alpha, int beta, int d) {
	TableEntry entry = mTable[mpBoard->zobristKey % mSize];

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
		stats.numHits++;
		return entry.value;
	}

	if (entry.type == upperBound && entry.value <= alpha) {
		stats.numHits++;
		return entry.value;
	}

	if (entry.type == lowerBound && entry.value >= beta) {
		stats.numHits++;
		return entry.value;
	}

	return invalidEval;
}

Move TranspositionTable::getMove() {
	return mTable[mpBoard->zobristKey % mSize].move;
}

TranspositionTable::TableEntry TranspositionTable::getEntry() {
	return mTable[mpBoard->zobristKey % mSize];
}

void TranspositionTable::clear() {
	for (int i = 0; i < mSize; i++)
		mTable[i] = TableEntry();
}

void TranspositionTable::resetStats() {
	stats = TableStats();
}

TranspositionTable::TableEntry::TableEntry()
	: zobrist(0), type(invalid), value(0), depth(0), move(0) {}

TranspositionTable::TableEntry::TableEntry(Uint64 zKey, TableEntryType t, int v, int d, Move m)
	: zobrist(zKey), type(t), value(v), depth(d), move(m) {}