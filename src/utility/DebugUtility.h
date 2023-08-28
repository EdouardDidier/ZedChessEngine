#pragma once

#include "./src/engine/Board.h"
#include "./src/engine/PieceList.h"

class DebugUtility
{
public:
	static PieceList **getPieceListFromSelector(Board *pBoard, int selector);
};

