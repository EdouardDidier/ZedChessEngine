#include "EvaluationOld.h"

int EvaluationV2::evaluate(Board* pBoard) {
	int friendlyIndex = pBoard->colourToMoveIndex;
	int opponentIndex = pBoard->opponentColourIndex;
	int eval = 0;

	eval += (pBoard->pawns[friendlyIndex]->count() - pBoard->pawns[opponentIndex]->count()) * EvaluationV2::pawnValue;
	eval += (pBoard->knights[friendlyIndex]->count() - pBoard->knights[opponentIndex]->count()) * EvaluationV2::knightValue;
	eval += (pBoard->bishops[friendlyIndex]->count() - pBoard->bishops[opponentIndex]->count()) * EvaluationV2::bishopValue;
	eval += (pBoard->rooks[friendlyIndex]->count() - pBoard->rooks[opponentIndex]->count()) * EvaluationV2::rookValue;
	eval += (pBoard->queens[friendlyIndex]->count() - pBoard->queens[opponentIndex]->count()) * EvaluationV2::queenValue;

	return eval;
}

int EvaluationV3::evaluate(Board* pBoard) {
	int friendlyIndex = pBoard->colourToMoveIndex;
	int opponentIndex = pBoard->opponentColourIndex;
	int eval = 0;

	eval += (pBoard->pawns[friendlyIndex]->count() - pBoard->pawns[opponentIndex]->count()) * EvaluationV3::pawnValue;
	eval += (pBoard->knights[friendlyIndex]->count() - pBoard->knights[opponentIndex]->count()) * EvaluationV3::knightValue;
	eval += (pBoard->bishops[friendlyIndex]->count() - pBoard->bishops[opponentIndex]->count()) * EvaluationV3::bishopValue;
	eval += (pBoard->rooks[friendlyIndex]->count() - pBoard->rooks[opponentIndex]->count()) * EvaluationV3::rookValue;
	eval += (pBoard->queens[friendlyIndex]->count() - pBoard->queens[opponentIndex]->count()) * EvaluationV3::queenValue;

	return eval;
}

int EvaluationV4::evaluate(Board* pBoard) {
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