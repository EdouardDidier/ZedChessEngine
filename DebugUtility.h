#pragma once

#include "Board.h"
#include "PieceList.h"

class DebugUtility
{
public:
	static PieceList **getPieceListFromSelector(Board *pBoard, int selector);
};

