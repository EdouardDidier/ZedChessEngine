#include "Coord.h"

Coord::Coord(int index) {
	mFileIndex = index / 8;
	mRankIndex = index % 8;
}

Coord::Coord(int file, int rank) {
	mFileIndex = file;
	mRankIndex = rank;
}

Coord::~Coord() {

}

int Coord::getRank() {
	return mFileIndex;
}

int Coord::getFile() {
	return mRankIndex;
}

bool Coord::isInBoard() {
	return mFileIndex > -1 && mRankIndex > -1;
}

int Coord::getIndex() {
	return mFileIndex * 8 + mRankIndex;
}

bool Coord::compareTo(Coord pOther) {
	return (mFileIndex == pOther.getRank() && mRankIndex == pOther.getFile()) ? 1 : 0;
}
