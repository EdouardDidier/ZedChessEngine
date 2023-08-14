#include "BoardRepresentation.h"

string BoardRepresentation::getMoveString(Move move) {
	return getSquareString(move.getStartSquare()) + getSquareString(move.getTargetSquare());
}

string BoardRepresentation::getSquareString(int index) {
	return getSquareString(index / 8, index % 8);
}

string BoardRepresentation::getSquareString(int file, int rank) {
	string str = "a1";
	str[0] += rank;
	str[1] += file;
	return str;
}

int BoardRepresentation::getSquareFromString(string str) {
	if (str.size() != 2)
		return 0;

	int x = str[0] - 'a';
	int y = str[1] - '1';

	if (x < 0 || x > 7 || y < 0 || y > 7)
		return 0;

	return x + y * 8;
}

Move BoardRepresentation::getMoveFromString(string str) {
	if (str.size() != 4)
		return Move(0);

	int startSquare = getSquareFromString(str.substr(0, 2));
	int targetSquare = getSquareFromString(str.substr(2, 2));

	return Move(startSquare, targetSquare);
}