#pragma once

#include <iostream>
#include <string>

#include "Move.h"

using namespace std;

class BoardRepresentation
{
public:
	static string getMoveString(Move move);
	static string getSquareString(int index);
	static string getSquareString(int file, int rank);

	static int getSquareFromString(string str);
	static Move getMoveFromString(string str);
};

