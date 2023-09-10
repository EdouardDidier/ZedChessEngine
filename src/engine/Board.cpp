#include "Board.h"

Board::Board() {
	Zobrist::init();

	squares = new int[64];

	whiteToMove = true;
	colourToMove = Piece::white;
	opponentColour = Piece::black;
	colourToMoveIndex = 0;
	opponentColourIndex = 1;

	currentGameState = 0;
	zobristKey = 0;

	moveCount = 1;
	fiftyMoveCounter = 0;

	createPieceArrays();
}

Board::~Board() {
	delete[] squares;
	
	deletePieceArrays();
}

Board::Board(const Board& other)
	: squares(new int[64]), 
	whiteToMove(other.whiteToMove), 
	colourToMove(other.colourToMove), 
	opponentColour(other.opponentColour), 
	colourToMoveIndex(other.colourToMoveIndex), 
	opponentColourIndex(other.opponentColourIndex),
	currentGameState(other.currentGameState),
	gameStateHistory(other.gameStateHistory),
	moveHistory(other.moveHistory),
	moveToRedo(other.moveToRedo),
	zobristKey(other.zobristKey),
	zobristKeyHistory(other.zobristKeyHistory),
	repetitionHistory(other.repetitionHistory),
	moveCount(other.moveCount),
	fiftyMoveCounter(other.fiftyMoveCounter)
{
	for (int i = 0; i < 64; i++) {
		squares[i] = other.squares[i];
	}

	createPieceArrays();

	for (int i = 0; i < 2; i++) {
		kings[i] = other.kings[i];
	}

	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < other.mAllPieceLists[i]->count(); j++) {
			mAllPieceLists[i]->addAtSquare((*other.mAllPieceLists[i])[j]);	
		}
	}
}

void Board::makeMove(Move move, bool eraseMoveToRedo) {
	int oldEpFile = (currentGameState >> 4) & 0b1111;
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

	//cout << "(" << move.getStartSquare() << ", " << move.getTargetSquare() << ")" << endl;

	currentGameState = 0;

	// Erase history if requested
	if (eraseMoveToRedo)
		moveToRedo.clear();

	// Handeling captured piece, en-passant capture are managed later
	currentGameState |= (Uint32)capturedPiece << 8;
	if ((capturedPiece != Piece::none) && !move.isEnPassant()) {
		getPieceList(capturedPieceType, capturedPieceColour)->removeAtSquare(targetSquare);
		zobristKey ^= Zobrist::pieceKeys[opponentColourIndex][capturedPieceType][targetSquare];	// Removing captured piece from Zobrist key
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
		
		if (startSquare == 7 || targetSquare == 7) // 7 = h1
			newCastleState &= whiteCastleKingSideMask;
		
		if (startSquare == 56 || targetSquare == 56) // 56 = a8
			newCastleState &= blackCastleQueenSideMask;
		
		if (startSquare == 63 || targetSquare == 63) // 63 = h8
			newCastleState &= blackCastleKingSideMask;
	}

	// Handle Castle moves
	if (move.isCastle()) {
		Move rookMove = Move::getCastleRookMove(move);
		int rookStartSquare = rookMove.getStartSquare();
		int rookTargetSquare = rookMove.getTargetSquare();

		// Update PieceList
		rooks[colourToMoveIndex]->movePiece(rookStartSquare, rookTargetSquare);

		// Update board
		squares[rookTargetSquare] = squares[rookStartSquare];
		squares[rookStartSquare] = Piece::none;

		// Update Zobrist key
		zobristKey ^= Zobrist::pieceKeys[colourToMoveIndex][Piece::rook][rookStartSquare];
		zobristKey ^= Zobrist::pieceKeys[colourToMoveIndex][Piece::rook][rookTargetSquare];
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
		// Defining ep pawn square (pawn captured)
		int epPawnSquare = targetSquare + (whiteToMove ? -8 : 8);

		// Update game state
		currentGameState |= (Piece::pawn | opponentColour) << 8;

		// Update PieceList
		pawns[opponentColourIndex]->removeAtSquare(epPawnSquare);

		// Update Board
		squares[epPawnSquare] = Piece::none;

		// Update Zobrist Key
		zobristKey ^= Zobrist::pieceKeys[opponentColourIndex][Piece::pawn][epPawnSquare];
	}

	// Pawn has moved two forwards, mark file with en-passant flag
	if (moveFlag == Move::Flag::pawnTwoForward) {
		int file = Coord(move.getStartSquare()).getFile() + 1; //TODO refactor with real dedicated function for that purpose (Utility board representation static ?)
		currentGameState |= (Uint32)(file << 4);
		zobristKey ^= Zobrist::epKeys[file];
	}

	// Update Zobrist key
	zobristKey ^= Zobrist::sideToMoveKey;
	zobristKey ^= Zobrist::pieceKeys[colourToMoveIndex][pieceType][startSquare];
	zobristKey ^= Zobrist::pieceKeys[colourToMoveIndex][Piece::pieceType(squares[targetSquare])][targetSquare];

	// Removing old ep file in Zobrist key if needed
	if (oldEpFile != 0)
		zobristKey ^= Zobrist::epKeys[oldEpFile];

	// Updating castle state in Zobrist key if needed
	if (newCastleState != oldCastleState) {
		zobristKey ^= Zobrist::castleKeys[oldCastleState]; // Removing old castle rights
		zobristKey ^= Zobrist::castleKeys[newCastleState]; // Adding new castle rights
	}

	currentGameState |= (Uint32)newCastleState;

	// Save move to history
	gameStateHistory.push_back(currentGameState);
	moveHistory.push_back(move);
	zobristKeyHistory.push_back(zobristKey);
	repetitionHistory.push_back(zobristKey);

	// Change side to move
	whiteToMove = !whiteToMove;
	colourToMove = whiteToMove ? Piece::white : Piece::black;
	opponentColour = whiteToMove ? Piece::black : Piece::white;
	
	opponentColourIndex = colourToMoveIndex;
	colourToMoveIndex = 1 - colourToMoveIndex;

	moveCount++;
}

void Board::redoMove() {
	if (moveToRedo.empty())
		return;

	Move move = moveToRedo.back();
	moveToRedo.pop_back();

	makeMove(move, false);
}

void Board::undoMove() {
	// Handeling history
	if (moveHistory.empty())
		return;

	Move move = moveHistory.back();
	moveHistory.pop_back();

	moveToRedo.push_back(move);

	// Preparing datas
	opponentColourIndex = colourToMoveIndex;
	colourToMove = opponentColour;
	opponentColour = whiteToMove ? Piece::white : Piece::black;
	colourToMoveIndex = 1 - colourToMoveIndex;
	whiteToMove = !whiteToMove;

	Uint32 currentCastleState = currentGameState & 0b1111;

	int capturedPieceType = ((int)(currentGameState >> 8) & 0b111);
	int capturedPiece = (capturedPieceType == 0) ? 0 : (capturedPieceType | opponentColour);

	int startSquare = move.getStartSquare();
	int targetSquare = move.getTargetSquare();
	int moveFlags = move.getFlag();

	int finalPieceType = Piece::pieceType(squares[targetSquare]);
	int pieceType = move.isPromotion() ? Piece::pawn : finalPieceType;

	// Handle regular capture
	if (capturedPieceType != Piece::none && !move.isEnPassant()) {
		getPieceList(capturedPieceType, opponentColour)->addAtSquare(targetSquare);
	}

	// Handle piece move
	if (pieceType == Piece::king) {
		kings[colourToMoveIndex] = startSquare;
	}
	else if (!move.isPromotion()) {
		getPieceList(pieceType, colourToMove)->movePiece(targetSquare, startSquare);
	}

	squares[startSquare] = pieceType | colourToMove;
	squares[targetSquare] = capturedPiece;

	// Handle special moves
	if (move.isPromotion()) {
		pawns[colourToMoveIndex]->addAtSquare(startSquare);
		switch (moveFlags)
		{
		case Move::Flag::promoteToQueen:
			queens[colourToMoveIndex]->removeAtSquare(targetSquare);
			break;
		case Move::Flag::promoteToRook:
			rooks[colourToMoveIndex]->removeAtSquare(targetSquare);
			break;
		case Move::Flag::promoteToBishop:
			bishops[colourToMoveIndex]->removeAtSquare(targetSquare);
			break;
		case Move::Flag::promoteToKnight:
			knights[colourToMoveIndex]->removeAtSquare(targetSquare);
			break;
		}
	}
	else if (move.isEnPassant()) {
		int epSquare = targetSquare + ((whiteToMove ? -8 : 8));
		squares[targetSquare] = Piece::none;
		squares[epSquare] = capturedPiece;
		pawns[opponentColourIndex]->addAtSquare(epSquare);
	}
	else if (move.isCastle()) {
		// King side castle move are 6 = g1 and 62 = g8
		bool isKingSide = targetSquare == 6 || targetSquare == 62;
		int rookStartSquare = isKingSide ? targetSquare + 1 : targetSquare - 2;
		int rookTargetSquare = isKingSide ? targetSquare - 1 : targetSquare + 1;

		rooks[colourToMoveIndex]->movePiece(rookTargetSquare, rookStartSquare);

		squares[rookStartSquare] = squares[rookTargetSquare];
		squares[rookTargetSquare] = Piece::none;
	}

	gameStateHistory.pop_back();
	currentGameState = gameStateHistory.back();

	zobristKeyHistory.pop_back();
	zobristKey = zobristKeyHistory.back();

	if (!repetitionHistory.empty())
		repetitionHistory.pop_back();

	moveCount--;
}

Move Board::getFirstRedoMove() {
	return moveToRedo.empty() ? Move(0) : moveToRedo.back();
}

Move Board::getLastMove() {
	return moveHistory.empty() ? Move(0) : moveHistory.back();
}

int Board::getPiece(int index) {
	return squares[index];
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

bool Board::isRepetition(int maxCount) {
	int count = 0;
	
	for (Uint64 key : repetitionHistory) {
		if (zobristKey == key)
			count++;
		
		if (count == maxCount) // 3 Because current key is included in the history
			return true;
	}

	return false;
}

void Board::reset() {
	deletePieceArrays();
	createPieceArrays();

	gameStateHistory.clear();
	zobristKeyHistory.clear();
	repetitionHistory.clear();

	loadStartPosition();
}

void Board::loadStartPosition() {
	loadPosition(Fen::fenStartPosition);
}

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
	opponentColourIndex = 1 - colourToMoveIndex;

	moveCount = position.moveCount;
	fiftyMoveCounter = position.fiftyMoveCounter;

	// Create gamestate
	int whiteCastle = ((position.whiteCastleKingside) ? 1 << 0 : 0) | ((position.whiteCastleQueenside) ? 1 << 1 : 0);
	int blackCastle = ((position.blackCastleKingside) ? 1 << 2 : 0) | ((position.blackCastleQueenside) ? 1 << 3 : 0);

	// En-passant
	int epState = position.epFile << 4;

	currentGameState = (int)(whiteCastle | blackCastle | epState);
	gameStateHistory.push_back(currentGameState);

	zobristKey = Zobrist::calculateKey(this);
	zobristKeyHistory.push_back(zobristKey);
	repetitionHistory.push_back(zobristKey);
}

void Board::createPieceArrays() {
	// Allocating memory for list of pieces
	kings = new int[2];

	queens = new PieceList * [2];
	rooks = new PieceList * [2];
	bishops = new PieceList * [2];
	knights = new PieceList * [2];
	pawns = new PieceList * [2];
	PieceList** emptyPieceList = new PieceList * [2];

	for (int i = 0; i < 2; i++) {
		queens[i] = new PieceList(9);
		rooks[i] = new PieceList(10);
		bishops[i] = new PieceList(10);
		knights[i] = new PieceList(10);
		pawns[i] = new PieceList(8);
		emptyPieceList[i] = new PieceList(0);
	}

	mAllPieceLists = new PieceList * [16] {
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

void Board::deletePieceArrays() {
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
