#include "Board.h"

Board::Board() {
	squares = new int[64];

	whiteToMove = true;
	colourToMove = Piece::white;
	opponentColour = Piece::black;
	colourToMoveIndex = 0;
	opponentColourIndex = 1;

	currentGameState = 0;

	moveCount = 1;
	fiftyMoveCounter = 0;

	// Allocating memory for list of pieces
	kings = new int[2];

	queens = new PieceList *[2];
	rooks = new PieceList *[2];
	bishops = new PieceList *[2];
	knights = new PieceList *[2];
	pawns = new PieceList *[2];
	PieceList **emptyPieceList = new PieceList *[2];
	
	for (int i = 0; i < 2; i++) {
		queens[i] = new PieceList(9);
		rooks[i] = new PieceList(10);
		bishops[i] = new PieceList(10);
		knights[i] = new PieceList(10);
		pawns[i] = new PieceList(8);
		emptyPieceList[i] = new PieceList(0);
	}

	mAllPieceLists = new PieceList *[16] {
		emptyPieceList[whiteIndex],
		emptyPieceList[whiteIndex],
		pawns[whiteIndex],
		knights[whiteIndex],
		emptyPieceList[whiteIndex],
		bishops[whiteIndex],
		rooks[whiteIndex],
		queens[whiteIndex],
		emptyPieceList[blackIndex],
		emptyPieceList[blackIndex],
		pawns[blackIndex],
		knights[blackIndex],
		emptyPieceList[blackIndex],
		bishops[blackIndex],
		rooks[blackIndex],
		queens[blackIndex]
	};
}

Board::~Board() {
	delete squares;

	// Clearing memory for list of pieces
	for (int i = 0; i < 2; i++) {
		delete queens[i];
		delete rooks[i];
		delete bishops[i];
		delete knights[i];
		delete pawns[i];
		delete mAllPieceLists[i * 8];
	}

	delete kings;

	delete queens;
	delete rooks;
	delete bishops;
	delete knights;
	delete pawns;

	delete mAllPieceLists;
}

void Board::makeMove(Move move) {
	int oldCastleState = currentGameState & 0b1111;
	int newCastleState = oldCastleState;

	int startSquare = move.getStartSquare();
	int targetSquare = move.getTargetSquare();
	int moveFlag = move.getFlag();

	int piece = squares[startSquare];
	int pieceType = Piece::pieceType(piece);
	int pieceColour = Piece::colour(piece);

	int capturedPiece = squares[targetSquare];
	int capturedPieceType = Piece::pieceType(capturedPiece);
	int capturedPieceColour = Piece::colour(capturedPiece);

	currentGameState = 0;

	//cout << "(" << move.getStartSquare() << ", " << move.getTargetSquare() << ")" << endl;

	// Handeling captured piece
	currentGameState |= capturedPiece << 8;
	if ((capturedPiece != Piece::none) && !move.isEnPassant()) {
		getPieceList(capturedPieceType, capturedPieceColour)->removeAtSquare(targetSquare);
	}

	// Update Piece List and Update Caste state
	if (Piece::pieceType(piece) == Piece::king) {
		newCastleState &= whiteToMove ? whiteCastleMask : blackCastleMask;
		kings[colourToMoveIndex] = targetSquare;
	}
	else {
		getPieceList(pieceType, pieceColour)->movePiece(startSquare, targetSquare);
	}

	// Update board
	squares[targetSquare] = squares[startSquare];
	squares[startSquare] = Piece::none;

	// If rooks move or get captured it removes castle rights
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

		// Update PieceList
		rooks[colourToMoveIndex]->movePiece(rookMove.getStartSquare(), rookMove.getTargetSquare());

		// Update board
		squares[rookMove.getTargetSquare()] = squares[rookMove.getStartSquare()];
		squares[rookMove.getStartSquare()] = Piece::none;
	} 
	// Handle Promotions
	else if (move.isPromotion()) {
		int promotionType = 0;

		switch (moveFlag) {
		case Move::Flag::promoteToQueen:
			promotionType = Piece::queen;
			break;
		case Move::Flag::promoteToRook:
			promotionType = Piece::rook;
			break;
		case Move::Flag::promoteToBishop:
			promotionType = Piece::bishop;
			break;
		case Move::Flag::promoteToKnight:
			promotionType = Piece::knight;
			break;
		}

		// Update PieceList
		getPieceList(promotionType, pieceColour)->addAtSquare(targetSquare);
		pawns[colourToMoveIndex]->removeAtSquare(targetSquare);

		// Update Board
		squares[targetSquare] = promotionType | (whiteToMove ? Piece::white : Piece::black);
	}
	// Handle captures en-passant
	else if (move.isEnPassant()) {
		// Update game state
		currentGameState |= (Piece::pawn | opponentColour) << 8;

		// Update PieceList
		pawns[opponentColourIndex]->removeAtSquare(targetSquare + (whiteToMove ? -8 : 8));

		// Update Board
		squares[targetSquare + (whiteToMove ? -8 : 8)] = Piece::none;
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
	
	opponentColourIndex = colourToMoveIndex;
	colourToMoveIndex = 1 - colourToMoveIndex;

	currentGameState |= newCastleState;
}

int Board::getPiece(int index) {
	return this->getPiece(Coord(index));
}

int Board::getPiece(int fileIndex, int rankIndex) {
	if (fileIndex < 0 || fileIndex > 7 || rankIndex < 0 || rankIndex > 7)
		return -1;

	return squares[fileIndex * 8 + rankIndex];
}

int Board::getPiece(Coord coord) {
	return this->getPiece(coord.getRank(), coord.getFile());
}

PieceList *Board::getPieceList(int pieceType, int pieceColour) {
	return mAllPieceLists[pieceType + pieceColour - 8];
}

// Load starting position
void Board::loadStartPosition() {
	loadPosition(Fen::fenStartPosition);
}

// Load a position from a fen input
void Board::loadPosition(string fen) {
	Fen::Position position = Fen::positionFromFen(fen);

	for (int i = 0; i < 64; i++) {
		squares[i] = position.squares[i];
		
		int pieceType = Piece::pieceType(squares[i]);
		int colourIndex = Piece::isColour(squares[i], Piece::white) ? whiteIndex : blackIndex;

		switch (pieceType)
		{
		case Piece::king:
			kings[colourIndex] = i;
			break;
		case Piece::queen:
			queens[colourIndex]->addAtSquare(i);
			break;
		case Piece::rook:
			rooks[colourIndex]->addAtSquare(i);
			break;
		case Piece::bishop:
			bishops[colourIndex]->addAtSquare(i);
			break;
		case Piece::knight:
			knights[colourIndex]->addAtSquare(i);
			break;
		case Piece::pawn:
			pawns[colourIndex]->addAtSquare(i);
			break;
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
