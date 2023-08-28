#include "Evaluation.h"

int EvaluationV4::evaluate(Board *pBoard) {
	int friendlyIndex = pBoard->colourToMoveIndex;
	int opponentIndex = pBoard->opponentColourIndex;
	int eval = 0;

	eval += (pBoard->pawns[friendlyIndex]->count() - pBoard->pawns[opponentIndex]->count()) * EvaluationV4::pawnValue;
	eval += (pBoard->knights[friendlyIndex]->count() - pBoard->knights[opponentIndex]->count()) * EvaluationV4::knightValue;
	eval += (pBoard->bishops[friendlyIndex]->count() - pBoard->bishops[opponentIndex]->count()) * EvaluationV4::bishopValue;
	eval += (pBoard->rooks[friendlyIndex]->count() - pBoard->rooks[opponentIndex]->count()) * EvaluationV4::rookValue;
	eval += (pBoard->queens[friendlyIndex]->count() - pBoard->queens[opponentIndex]->count()) * EvaluationV4::queenValue;

	return eval;
}