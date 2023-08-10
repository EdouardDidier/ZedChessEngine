#include "DebugUtility.h"

PieceList **DebugUtility::getPieceListFromSelector(Board* pBoard, int selector) {
	switch (selector)
	{
	case 2:
		return pBoard->queens;
	case 3:
		return pBoard->rooks;
	case 4:
		return pBoard->bishops;
	case 5:
		return pBoard->knights;
	}
	
	return pBoard->pawns;
}
