#include "Evaluation.h"

int EvaluationV5::evaluate(Board *pBoard) {
	int friendlyIndex = pBoard->colourToMoveIndex;
	int opponentIndex = pBoard->opponentColourIndex;
	int eval = 0;

	eval += (pBoard->pawns[friendlyIndex]->count() - pBoard->pawns[opponentIndex]->count()) * EvaluationV5::pawnValue;
	eval += (pBoard->knights[friendlyIndex]->count() - pBoard->knights[opponentIndex]->count()) * EvaluationV5::knightValue;
	eval += (pBoard->bishops[friendlyIndex]->count() - pBoard->bishops[opponentIndex]->count()) * EvaluationV5::bishopValue;
	eval += (pBoard->rooks[friendlyIndex]->count() - pBoard->rooks[opponentIndex]->count()) * EvaluationV5::rookValue;
	eval += (pBoard->queens[friendlyIndex]->count() - pBoard->queens[opponentIndex]->count()) * EvaluationV5::queenValue;

	return eval;
}