#include "CLI.h"

CLI::CLI(Board *pBoard) {
	pBoard = pBoard;
}

CLI::~CLI() {

}

bool  CLI::init() {
	// Initialize SDL. If failed, end program.
	if (SDL_Init(SDL_INIT_TIMER) < 0) {
		cout << "Error initializing SDL: " << SDL_GetError() << endl;
		system("pause");
		return false;
	}

	// Load start position from Fen string
	mpBoard->loadStartPosition(); //TODO : adapt to test any positions with dataset

	cout << "ZedChessEngine" << endl;

	return true;
}

void CLI::kill() {
	
}

void CLI::run() {
	bool quit = false;

	while (!quit) {
		string token = "";

		cin >> token;

		if (token == "quit" || token == "q") {
			quit = true;
		}
		else if (token == "display" || token == "d") {
			displayBoard();
		}
		else if (token == "graphic" || token == "g") {
			if (!startGraphicUI())
				cout << "Error starting graphic interface" << endl;
		}
		else if (token == "move" || token == "m") {
			string moveStr = "";
			cin >> moveStr;

			makeMove(moveStr);
		}
		else if (token == "redo") {
			redoMove();
		}
		else if (token == "undo") {
			undoMove();
		}
		else if (token == "reset" || token == "r") {
			mpBoard->reset();
		}
		else if (token == "fen") {
			string fenStr = "";
			getline(cin, fenStr);
			fenStr.erase(0, 1);

			loadFen(fenStr);
		}
		else if (token == "legal") {
			displayLegalMove();
		}
		else if (token == "perft") {
			int depth = 0;
			cin >> depth;

			if (depth < 1)
				depth = 1;

			Perft perft;

			perft.runTestDivided(mpBoard, depth);
		}
	}
}

void CLI::loadFen(string str) {
	delete mpBoard;
	mpBoard = new Board();
	mpBoard->loadPosition(str);
}

void CLI::makeMove(string str) {
	vector<Move> moves = mMoveGenerator.generateLegalMove(mpBoard);

	Move requestedMove = BoardRepresentation::getMoveFromString(str);

	if (!requestedMove.isInvalid()) {
		for (Move move : moves) {
			if (Move::isSameMoveNoFlag(move, requestedMove)) {
				mpBoard->makeMove(move); // Promotion, not handle
				return;
			}
		}
	}

	cout << "Move is illegal" << endl;
}

void CLI::redoMove() {
	mpBoard->redoMove();
}

void CLI::undoMove() {
	mpBoard->undoMove();
}

void CLI::displayLegalMove() {
	vector<Move> moves = mMoveGenerator.generateLegalMove(mpBoard);

	for (Move move : moves) {
		cout << BoardRepresentation::getMoveString(move) << " ";
	}

	cout << endl;
}

void CLI::displayBoard() {
	cout << endl << " +---+---+---+---+---+---+---+---+" << endl;

	for (int y = 0; y < 8; y++) {
		
		cout << " |";
		for (int x = 0; x < 8; x++) {
			cout << " " << getPieceChar(x + (7 - y) * 8) << " |";
		}

		cout << " " << (8 - y) << endl;
		cout << " +---+---+---+---+---+---+---+---+" << endl;
	}

	cout << "   a   b   c   d   e   f   g   h  " << endl << endl;
}

char CLI::getPieceChar(int index) {
	int piece = mpBoard->getPiece(index);
	int pieceType = Piece::pieceType(piece);
	int colour = Piece::colour(piece);

	char result = ' ';

	switch (pieceType)
	{
	case Piece::king:
		result = 'k';
		break;
	case Piece::queen:
		result = 'q';
		break;
	case Piece::rook:
		result = 'r';
		break;
	case Piece::bishop:
		result = 'b';
		break;
	case Piece::knight:
		result = 'n';
		break;
	case Piece::pawn:
		result = 'p';
		break;
	}

	if (colour == Piece::white)
		result = toupper(result);

	return result;
}

bool CLI::startGraphicUI() {
	Game* pGame = new Game(mpBoard);

	// Initializing the game. If failed end the program with error code.
	if (!pGame->init()) return false;

	pGame->run();

	// Freeing memory
	pGame->kill();
	delete pGame;

	return true;
}