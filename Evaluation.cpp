#include "Evaluation.h"

int Evaluation::evaluate(Board *pBoard) {
	int friendlyIndex = pBoard->colourToMoveIndex;
	int opponentIndex = pBoard->opponentColourIndex;
	int eval = 0;

	eval += (pBoard->pawns[friendlyIndex]->count() - pBoard->pawns[opponentIndex]->count()) * Evaluation::pawnValue;
	eval += (pBoard->knights[friendlyIndex]->count() - pBoard->knights[opponentIndex]->count()) * Evaluation::knightValue;
	eval += (pBoard->bishops[friendlyIndex]->count() - pBoard->bishops[opponentIndex]->count()) * Evaluation::bishopValue;
	eval += (pBoard->rooks[friendlyIndex]->count() - pBoard->rooks[opponentIndex]->count()) * Evaluation::rookValue;
	eval += (pBoard->queens[friendlyIndex]->count() - pBoard->queens[opponentIndex]->count()) * Evaluation::queenValue;

	return eval;
}