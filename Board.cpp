#include "Board.h"

Board::Board() {
	pSquares = new int[64];

	whiteToMove = true;
	colourToMove = Piece::white;
	opponentColour = Piece::black;
	colourToMoveIndex = 0;

	currentGameState = 0;

	moveCount = 1;
	fiftyMoveCounter = 0;
}

Board::~Board() {
	delete pSquares;
}

void Board::makeMove(Move move) {
	int oldCastleState = currentGameState & 0b1111;
	int newCastleState = oldCastleState;

	int startSquare = move.getStartSquare();
	int targetSquare = move.getTargetSquare();
	int piece = this->pSquares[startSquare];
	int moveFlag = move.getFlag();

	currentGameState = 0; // TODO, manage better or get data before reset

	//cout << "(" << move.getStartSquare() << ", " << move.getTargetSquare() << ")" << endl;
	this->pSquares[targetSquare] = this->pSquares[startSquare];
	this->pSquares[startSquare] = Piece::none;

	// Update Caste state
	if (Piece::pieceType(piece) == Piece::king) {
		newCastleState &= whiteToMove ? whiteCastleMask : blackCastleMask;
	}

	// If Rooks move or get captured it removes castle rights
	if (oldCastleState != 0) {
		if (startSquare == 0 || targetSquare == 0) // 0 = a1
			newCastleState &= whiteCastleQueenSideMask;
		else if (startSquare == 7 || targetSquare == 7) // 7 = h1
			newCastleState &= whiteCastleKingSideMask;
		else if (startSquare == 56 || targetSquare == 56) // 56 = a8
			newCastleState &= blackCastleQueenSideMask;
		else if (startSquare == 63 || targetSquare == 63) // 63 = h8
			newCastleState &= blackCastleKingSideMask;
	}

	// Handle Castle moves
	if (move.isCastle()) {
		Move rookMove = Move::getCastleRookMove(move);

		this->pSquares[rookMove.getTargetSquare()] = this->pSquares[rookMove.getStartSquare()];
		this->pSquares[rookMove.getStartSquare()] = Piece::none;
	} 
	// Handle Promotions
	else if (move.isPromotion()) {
		int promotionPiece = 0;

		switch (moveFlag) {
		case Move::Flag::promoteToQueen:
			promotionPiece = Piece::queen;
			break;
		case Move::Flag::promoteToRook:
			promotionPiece = Piece::rook;
			break;
		case Move::Flag::promoteToBishop:
			promotionPiece = Piece::bishop;
			break;
		case Move::Flag::promoteToKnight:
			promotionPiece = Piece::knight;
			break;
		}

		this->pSquares[targetSquare] = promotionPiece | (whiteToMove ? Piece::white : Piece::black);
	}
	// Handle captures en-passant
	else if (move.isEnPassant()) {
		if (whiteToMove) 
			this->pSquares[targetSquare - 8] = Piece::none;
		else
			this->pSquares[targetSquare + 8] = Piece::none;
	}

	// Pawn has moved two forwards, mark file with en-passant flag
	if (moveFlag == Move::Flag::pawnTwoForward) {
		int file = Coord(move.getStartSquare()).getFile(); //TODO refactor with real dedicated function for that purpose (Utility board representation static ?)
		currentGameState |= (short unsigned int)(file << 4);
	}

	// Change side to move
	whiteToMove = !whiteToMove;
	colourToMove = whiteToMove ? Piece::white : Piece::black;
	opponentColour = whiteToMove ? Piece::black : Piece::white;
	colourToMoveIndex = 1 - colourToMoveIndex;

	currentGameState |= newCastleState;
}

int Board::getPiece(int index) {
	return this->getPiece(Coord(index));
}

int Board::getPiece(int fileIndex, int rankIndex) {
	if (fileIndex < 0 || fileIndex > 7 || rankIndex < 0 || rankIndex > 7)
		return -1;

	return pSquares[fileIndex * 8 + rankIndex];
}

int Board::getPiece(Coord coord) {
	return this->getPiece(coord.getRank(), coord.getFile());
}

// Load starting position
void Board::loadStartPosition() {
	loadPosition(Fen::fenStartPosition);
}

// Load a position from a fen input
void Board::loadPosition(string fen) {
	Fen::Position position = Fen::positionFromFen(fen);

	for (int file = 0; file < 8; file++) {
		for (int rank = 0; rank < 8; rank++) {
			pSquares[file * 8 + rank] = position.squares[file * 8 + rank];
		}
	}

	whiteToMove = position.whiteToMove;
	colourToMove = whiteToMove ? Piece::white : Piece::black;
	opponentColour = whiteToMove ? Piece::black : Piece::white;
	colourToMoveIndex = whiteToMove ? 0 : 1;

	moveCount = position.moveCount;
	fiftyMoveCounter = position.fiftyMoveCounter;

	// Create gamestate
	int whiteCastle = ((position.whiteCastleKingside) ? 1 << 0 : 0) | ((position.whiteCastleQueenside) ? 1 << 1 : 0);
	int blackCastle = ((position.blackCastleKingside) ? 1 << 2 : 0) | ((position.blackCastleQueenside) ? 1 << 3 : 0);
	currentGameState = (int)(whiteCastle | blackCastle);
}
