#pragma once
class Coord
{
public:
	Coord(int index = -1);
	Coord(int file, int rank);
	~Coord();

	int getRank();
	int getFile();

	bool isInBoard();


	int getIndex();
	bool compareTo(Coord other);

private:
	int mFileIndex;
	int mRankIndex;
};

