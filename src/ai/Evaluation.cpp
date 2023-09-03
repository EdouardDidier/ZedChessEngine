#include "Evaluation.h"

EvaluationV5::EvaluationV5() {
	mpBoard = NULL;

	mFriendlyIndex = 0;
	mOpponentIndex = 1;
}

int EvaluationV5::evaluate(Board *pBoard) {
	mpBoard = pBoard; //TODO: Move into Constructor
	
	int eval = 0;

	mFriendlyIndex = mpBoard->colourToMoveIndex;
	mOpponentIndex = mpBoard->opponentColourIndex;

	eval += (mpBoard->pawns[mFriendlyIndex]->count() - mpBoard->pawns[mOpponentIndex]->count()) * EvaluationV5::pawnValue;
	eval += (mpBoard->knights[mFriendlyIndex]->count() - mpBoard->knights[mOpponentIndex]->count()) * EvaluationV5::knightValue;
	eval += (mpBoard->bishops[mFriendlyIndex]->count() - mpBoard->bishops[mOpponentIndex]->count()) * EvaluationV5::bishopValue;
	eval += (mpBoard->rooks[mFriendlyIndex]->count() - mpBoard->rooks[mOpponentIndex]->count()) * EvaluationV5::rookValue;
	eval += (mpBoard->queens[mFriendlyIndex]->count() - mpBoard->queens[mOpponentIndex]->count()) * EvaluationV5::queenValue;

	eval += evaluatePieceSquareTables();

	return eval;
}

int EvaluationV5::evaluatePieceSquareTables() {
	int eval = 0;

	//TODO: refactor to make it more simple
	//TODO: balance early and late game
	bool isWhite = mFriendlyIndex == mpBoard->whiteIndex;
	eval += evaluatePieceSquare(PieceSquareTables::pawn, mpBoard->pawns[mFriendlyIndex], isWhite);
	eval += evaluatePieceSquare(PieceSquareTables::knight, mpBoard->knights[mFriendlyIndex], isWhite);
	eval += evaluatePieceSquare(PieceSquareTables::bishop, mpBoard->bishops[mFriendlyIndex], isWhite);
	eval += evaluatePieceSquare(PieceSquareTables::queen, mpBoard->queens[mFriendlyIndex], isWhite);
	eval += PieceSquareTables::getSquareValue(PieceSquareTables::king_mg, mpBoard->kings[mFriendlyIndex], isWhite);

	eval -= evaluatePieceSquare(PieceSquareTables::pawn, mpBoard->pawns[mOpponentIndex], !isWhite);
	eval -= evaluatePieceSquare(PieceSquareTables::knight, mpBoard->knights[mOpponentIndex], !isWhite);
	eval -= evaluatePieceSquare(PieceSquareTables::bishop, mpBoard->bishops[mOpponentIndex], !isWhite);
	eval -= evaluatePieceSquare(PieceSquareTables::queen, mpBoard->queens[mOpponentIndex], !isWhite);
	eval -= PieceSquareTables::getSquareValue(PieceSquareTables::king_mg, mpBoard->kings[mOpponentIndex], !isWhite);

	return eval;
}

int EvaluationV5::evaluatePieceSquare(const int *table, PieceList *pieceList, bool isWhite) {
	int eval = 0;

	for (int i = 0; i < pieceList->count(); i++) {
		eval += PieceSquareTables::getSquareValue(table, (*pieceList)[i], isWhite);
	}

	return eval;
}